#include "lzma.h"
#include "edic_defs.h"
#include "sys.h"

#pragma arm section rodata = ".ARM.unlzma" code = ".ARM.unlzma" rwdata = ".ARM.unlzma" zidata = ".ARM.unlzma"  

#define	MIN(a, b) (((a) < (b)) ? (a) : (b))

/*static long long read_int(unsigned char *ptr, int size){
	int i;
	long long ret = 0;

	for (i = 0; i < size; i++)
		ret = (ret << 8) | ptr[size-i-1];
	return ret;
}*/

//#define ENDIAN_CONVERT(x) x = (typeof(x))read_int((unsigned char *)&x, sizeof(x))

//#define LZMA_IOBUF_SIZE	0x4000

//uint8_t buf[LZMA_IOBUF_SIZE];

struct rc {
	uint8_t *ptr;
	uint8_t *buffer;
//	uint8_t *buffer_end;
//	uint32_t buffer_size;
	uint32_t code;
	uint32_t range;
	uint32_t bound;
};


#define RC_TOP_BITS 24
#define RC_MOVE_BITS 5
#define RC_MODEL_TOTAL_BITS 11

/* Called once */
void rc_init(struct rc *rc, char *buffer){
	rc->buffer = (uint8_t *)buffer;
//	rc->buffer_size = buffer_size;
//	rc->buffer_end = rc->buffer + rc->buffer_size;
	rc->ptr = rc->buffer;

	rc->code = 0;
	rc->range = 0xFFFFFFFF;
}

void rc_init_code(struct rc *rc){
	int i;

	for (i = 0; i < 5; i++) rc->code = (rc->code << 8) | *rc->ptr++;
}


/* Called twice, but one callsite is in inline'd rc_is_bit_0_helper() */
void rc_do_normalize(struct rc *rc){
	rc->range <<= 8;
	rc->code = (rc->code << 8) | *rc->ptr++;
}
void rc_normalize(struct rc *rc){
	if (rc->range < (1 << RC_TOP_BITS))
		rc_do_normalize(rc);
}

/* Called 9 times */
/* Why rc_is_bit_0_helper exists?
 *Because we want to always expose (rc->code < rc->bound) to optimizer
 */
uint32_t rc_is_bit_0_helper(struct rc *rc, uint32_t *p){
	rc_normalize(rc);
	rc->bound = *p * (rc->range >> RC_MODEL_TOTAL_BITS);
	return rc->bound;
}
int rc_is_bit_0(struct rc *rc, uint32_t *p)
{
	uint32_t t = rc_is_bit_0_helper(rc, p);
	return rc->code < t;
}

/* Called ~10 times, but very small, thus inlined */
void rc_update_bit_0(struct rc *rc, uint32_t *p)
{
	rc->range = rc->bound;
	*p += ((1 << RC_MODEL_TOTAL_BITS) - *p) >> RC_MOVE_BITS;
}
void rc_update_bit_1(struct rc *rc, uint32_t *p)
{
	rc->range -= rc->bound;
	rc->code -= rc->bound;
	*p -= *p >> RC_MOVE_BITS;
}

/* Called 4 times in unlzma loop */
int rc_get_bit(struct rc *rc, uint32_t *p, int *symbol)
{
	if (rc_is_bit_0(rc, p)) {
		rc_update_bit_0(rc, p);
		*symbol *= 2;
		return 0;
	} else {
		rc_update_bit_1(rc, p);
		*symbol = *symbol * 2 + 1;
		return 1;
	}
}

/* Called once */
int rc_direct_bit(struct rc *rc)
{
	rc_normalize(rc);
	rc->range >>= 1;
	if (rc->code >= rc->range) {
		rc->code -= rc->range;
		return 1;
	}
	return 0;
}

/* Called twice */
void rc_bit_tree_decode(struct rc *rc, uint32_t *p, int num_levels, int *symbol)
{
	int i = num_levels;

	*symbol = 1;
	while (i--)
		rc_get_bit(rc, p + *symbol, symbol);
	*symbol -= 1 << num_levels;
}


/*
 * Small lzma deflate implementation.
 * Copyright (C) 2006  Aurelien Jacobs < aurel@gnuage.org >
 *
 * Based on LzmaDecode.c from the LZMA SDK 4.22 (http://www.7-zip.org/)
 * Copyright (C) 1999-2005  Igor Pavlov
 */


struct lzma_header {
	uint8_t pos;
	uint32_t dict_size;
	uint64_t dst_size;
};


#define LZMA_BASE_SIZE 1846
#define LZMA_LIT_SIZE 768

#define LZMA_NUM_POS_BITS_MAX 4

#define LZMA_LEN_NUM_LOW_BITS 3
#define LZMA_LEN_NUM_MID_BITS 3
#define LZMA_LEN_NUM_HIGH_BITS 8

#define LZMA_LEN_CHOICE 0
#define LZMA_LEN_CHOICE_2 (LZMA_LEN_CHOICE + 1)
#define LZMA_LEN_LOW (LZMA_LEN_CHOICE_2 + 1)
#define LZMA_LEN_MID (LZMA_LEN_LOW \
		      + (1 << (LZMA_NUM_POS_BITS_MAX + LZMA_LEN_NUM_LOW_BITS)))
#define LZMA_LEN_HIGH (LZMA_LEN_MID \
		       +(1 << (LZMA_NUM_POS_BITS_MAX + LZMA_LEN_NUM_MID_BITS)))
#define LZMA_NUM_LEN_PROBS (LZMA_LEN_HIGH + (1 << LZMA_LEN_NUM_HIGH_BITS))

#define LZMA_NUM_STATES 12
#define LZMA_NUM_LIT_STATES 7

#define LZMA_START_POS_MODEL_INDEX 4
#define LZMA_END_POS_MODEL_INDEX 14
#define LZMA_NUM_FULL_DISTANCES (1 << (LZMA_END_POS_MODEL_INDEX >> 1))

#define LZMA_NUM_POS_SLOT_BITS 6
#define LZMA_NUM_LEN_TO_POS_STATES 4

#define LZMA_NUM_ALIGN_BITS 4

#define LZMA_MATCH_MIN_LEN 2

#define LZMA_IS_MATCH 0
#define LZMA_IS_REP (LZMA_IS_MATCH + (LZMA_NUM_STATES << LZMA_NUM_POS_BITS_MAX))
#define LZMA_IS_REP_G0 (LZMA_IS_REP + LZMA_NUM_STATES)
#define LZMA_IS_REP_G1 (LZMA_IS_REP_G0 + LZMA_NUM_STATES)
#define LZMA_IS_REP_G2 (LZMA_IS_REP_G1 + LZMA_NUM_STATES)
#define LZMA_IS_REP_0_LONG (LZMA_IS_REP_G2 + LZMA_NUM_STATES)
#define LZMA_POS_SLOT (LZMA_IS_REP_0_LONG \
		       + (LZMA_NUM_STATES << LZMA_NUM_POS_BITS_MAX))
#define LZMA_SPEC_POS (LZMA_POS_SLOT \
		       +(LZMA_NUM_LEN_TO_POS_STATES << LZMA_NUM_POS_SLOT_BITS))
#define LZMA_ALIGN (LZMA_SPEC_POS \
		    + LZMA_NUM_FULL_DISTANCES - LZMA_END_POS_MODEL_INDEX)
#define LZMA_LEN_CODER (LZMA_ALIGN + (1 << LZMA_NUM_ALIGN_BITS))
#define LZMA_REP_LEN_CODER (LZMA_LEN_CODER + LZMA_NUM_LEN_PROBS)
#define LZMA_LITERAL (LZMA_REP_LEN_CODER + LZMA_NUM_LEN_PROBS)


struct writer {
	uint8_t *buffer;
	uint8_t previous_byte;
	size_t buffer_pos;
	int bufsize;
	struct lzma_header *header;
};

struct cstate {
	int state;
	uint32_t rep0, rep1, rep2, rep3;
};

size_t get_pos(struct writer *wr){
	return wr->buffer_pos;
}

uint8_t peek_old_byte(struct writer *wr, uint32_t offs){
		uint32_t pos = wr->buffer_pos - offs;
		while (pos >= wr->header->dict_size) pos += wr->header->dict_size;
		return wr->buffer[pos];
}

int write_byte(struct writer *wr, uint8_t byte){
	wr->buffer[wr->buffer_pos++] = wr->previous_byte = byte;
	return 0;
}


int copy_byte(struct writer *wr, uint32_t offs){
	return write_byte(wr, peek_old_byte(wr, offs));
}

int copy_bytes(struct writer *wr, uint32_t rep0, int len){
	do {
		if (copy_byte(wr, rep0))
			return -1;
		len--;
	} while (len != 0 && wr->buffer_pos < wr->header->dst_size);

	return len;
}

int process_bit0(struct writer *wr, struct rc *rc,
				     struct cstate *cst, uint32_t *p,
				     int pos_state, uint32_t *prob,
				     int lc, uint32_t literal_pos_mask) {
	int mi = 1;
	rc_update_bit_0(rc, prob);
	prob = (p + LZMA_LITERAL +
		(LZMA_LIT_SIZE
		 * (((get_pos(wr) & literal_pos_mask) << lc)
		    + (wr->previous_byte >> (8 - lc))))
		);

	if (cst->state >= LZMA_NUM_LIT_STATES) {
		int match_byte = peek_old_byte(wr, cst->rep0);
		do {
			int bit;
			uint32_t *prob_lit;

			match_byte <<= 1;
			bit = match_byte & 0x100;
			prob_lit = prob + 0x100 + bit + mi;
			if (rc_get_bit(rc, prob_lit, &mi)) {
				if (!bit)
					break;
			} else {
				if (bit)
					break;
			}
		} while (mi < 0x100);
	}
	while (mi < 0x100) {
		uint32_t *prob_lit = prob + mi;
		rc_get_bit(rc, prob_lit, &mi);
	}
	if (cst->state < 4)
		cst->state = 0;
	else if (cst->state < 10)
		cst->state -= 3;
	else
		cst->state -= 6;

	return write_byte(wr, mi);
}

int process_bit1(struct writer *wr, struct rc *rc,
					    struct cstate *cst, uint32_t *p,
					    int pos_state, uint32_t *prob) {
  int offset;
	uint32_t *prob_len;
	int num_bits;
	int len;

	rc_update_bit_1(rc, prob);
	prob = p + LZMA_IS_REP + cst->state;
	if (rc_is_bit_0(rc, prob)) {
		rc_update_bit_0(rc, prob);
		cst->rep3 = cst->rep2;
		cst->rep2 = cst->rep1; 
		cst->rep1 = cst->rep0;
		cst->state = cst->state < LZMA_NUM_LIT_STATES ? 0 : 3;
		prob = p + LZMA_LEN_CODER;
	} else {
		rc_update_bit_1(rc, prob);
		prob = p + LZMA_IS_REP_G0 + cst->state;
		if (rc_is_bit_0(rc, prob)) {
			rc_update_bit_0(rc, prob);
			prob = (p + LZMA_IS_REP_0_LONG
				+ (cst->state <<
				   LZMA_NUM_POS_BITS_MAX) +
				pos_state);
			if (rc_is_bit_0(rc, prob)) {
				rc_update_bit_0(rc, prob);

				cst->state = cst->state < LZMA_NUM_LIT_STATES ?
					9 : 11;
				return copy_byte(wr, cst->rep0);
			} else {
				rc_update_bit_1(rc, prob);
			}
		} else {
			uint32_t distance;

			rc_update_bit_1(rc, prob);
			prob = p + LZMA_IS_REP_G1 + cst->state;
			if (rc_is_bit_0(rc, prob)) {
				rc_update_bit_0(rc, prob);
				distance = cst->rep1;
			} else {
				rc_update_bit_1(rc, prob);
				prob = p + LZMA_IS_REP_G2 + cst->state;
				if (rc_is_bit_0(rc, prob)) {
					rc_update_bit_0(rc, prob);
					distance = cst->rep2;
				} else {
					rc_update_bit_1(rc, prob);
					distance = cst->rep3;
					cst->rep3 = cst->rep2;
				}
				cst->rep2 = cst->rep1;
			}
			cst->rep1 = cst->rep0;
			cst->rep0 = distance;
		}
		cst->state = cst->state < LZMA_NUM_LIT_STATES ? 8 : 11;
		prob = p + LZMA_REP_LEN_CODER;
	}

	prob_len = prob + LZMA_LEN_CHOICE;
	if (rc_is_bit_0(rc, prob_len)) {
		rc_update_bit_0(rc, prob_len);
		prob_len = (prob + LZMA_LEN_LOW
			    + (pos_state <<
			       LZMA_LEN_NUM_LOW_BITS));
		offset = 0;
		num_bits = LZMA_LEN_NUM_LOW_BITS;
	} else {
		rc_update_bit_1(rc, prob_len);
		prob_len = prob + LZMA_LEN_CHOICE_2;
		if (rc_is_bit_0(rc, prob_len)) {
			rc_update_bit_0(rc, prob_len);
			prob_len = (prob + LZMA_LEN_MID
				    + (pos_state <<
				       LZMA_LEN_NUM_MID_BITS));
			offset = 1 << LZMA_LEN_NUM_LOW_BITS;
			num_bits = LZMA_LEN_NUM_MID_BITS;
		} else {
			rc_update_bit_1(rc, prob_len);
			prob_len = prob + LZMA_LEN_HIGH;
			offset = ((1 << LZMA_LEN_NUM_LOW_BITS)
				  + (1 << LZMA_LEN_NUM_MID_BITS));
			num_bits = LZMA_LEN_NUM_HIGH_BITS;
		}
	}

	rc_bit_tree_decode(rc, prob_len, num_bits, &len);
	len += offset;

	if (cst->state < 4) {
		int pos_slot;

		cst->state += LZMA_NUM_LIT_STATES;
		prob =
			p + LZMA_POS_SLOT +
			((len <
			  LZMA_NUM_LEN_TO_POS_STATES ? len :
			  LZMA_NUM_LEN_TO_POS_STATES - 1)
			 << LZMA_NUM_POS_SLOT_BITS);
		rc_bit_tree_decode(rc, prob,
				   LZMA_NUM_POS_SLOT_BITS,
				   &pos_slot);
		if (pos_slot >= LZMA_START_POS_MODEL_INDEX) {
			int i, mi;
			num_bits = (pos_slot >> 1) - 1;
			cst->rep0 = 2 | (pos_slot & 1);
			if (pos_slot < LZMA_END_POS_MODEL_INDEX) {
				cst->rep0 <<= num_bits;
				prob = p + LZMA_SPEC_POS +
					cst->rep0 - pos_slot - 1;
			} else {
				num_bits -= LZMA_NUM_ALIGN_BITS;
				while (num_bits--)
					cst->rep0 = (cst->rep0 << 1) |
						rc_direct_bit(rc);
				prob = p + LZMA_ALIGN;
				cst->rep0 <<= LZMA_NUM_ALIGN_BITS;
				num_bits = LZMA_NUM_ALIGN_BITS;
			}
			i = 1;
			mi = 1;
			while (num_bits--) {
				if (rc_get_bit(rc, prob + mi, &mi))
					cst->rep0 |= i;
				i <<= 1;
			}
		} else
			cst->rep0 = pos_slot;
		if (++(cst->rep0) == 0)
			return 0;
		if (cst->rep0 > wr->header->dict_size
				|| cst->rep0 > get_pos(wr))
			return -1;
	}

	len += LZMA_MATCH_MIN_LEN;

	return copy_bytes(wr, cst->rep0, len);
}



int unlzma(uint8_t *buf, uint8_t *output, uint32_t out_len){
	struct lzma_header header;
	int lc, pb, lp;
	uint32_t pos_state_mask;
	uint32_t literal_pos_mask;
	uint32_t *p = (uint32_t *)VMEM_BASE;
	int num_probs;
	struct rc rc;
	int i, mi;
	struct writer wr;
	struct cstate cst;
	int ret = -1;

	cst.state = 0;
	cst.rep0 = cst.rep1 = cst.rep2 = cst.rep3 = 1;

	wr.header = &header;
	wr.previous_byte = 0;
	wr.buffer_pos = 0;

 	rc_init(&rc, (char *)buf);

	//for (i = 0; i < 13; i++) ((unsigned char *)&header)[i] = *rc.ptr++;
	header.pos = rc.ptr[0];
	header.dict_size = rc.ptr[1] + (rc.ptr[2] << 8) + (rc.ptr[3] << 16) + (rc.ptr[4] << 24);
	header.dst_size = rc.ptr[5] + (rc.ptr[6] << 8) + (rc.ptr[7] << 16) + (rc.ptr[8] << 24);
	rc.ptr += 13;

	if (header.pos >= (9 * 5 * 5)) {
		ret = -2;
		goto exit;
	}

	mi = 0;
	lc = header.pos;
	while (lc >= 9) {
		mi++;
		lc -= 9;
	}
	pb = 0;
	lp = mi;
	while (lp >= 5) {
		pb++;
		lp -= 5;
	}
	pos_state_mask = (1 << pb) - 1;
	literal_pos_mask = (1 << lp) - 1;

//	ENDIAN_CONVERT(header.dict_size);
//	ENDIAN_CONVERT(header.dst_size);

	if (header.dict_size == 0)
		header.dict_size = 1;

    wr.buffer = output;
    wr.bufsize = out_len;
    if (wr.buffer == 0)
		goto exit;

	num_probs = LZMA_LITERAL + (LZMA_LIT_SIZE << (lc + lp));
	for (i = 0; i < num_probs; i++)
		p[i] = (1 << RC_MODEL_TOTAL_BITS) >> 1;

	rc_init_code(&rc);

	while (get_pos(&wr) < header.dst_size) {
		int pos_state =	get_pos(&wr) & pos_state_mask;
		uint32_t *prob = p + LZMA_IS_MATCH +
			(cst.state << LZMA_NUM_POS_BITS_MAX) + pos_state;
		if (rc_is_bit_0(&rc, prob)) {
			if (process_bit0(&wr, &rc, &cst, p, pos_state, prob,
					lc, literal_pos_mask)) {
				ret = -3;
				goto exit;
			}
		} else {
			if (process_bit1(&wr, &rc, &cst, p, pos_state, prob)) {
				ret = -4;
				goto exit;
			}
			if (cst.rep0 == 0)
				break;
		}
//		if (rc.buffer_size <= 0)
//			goto exit;
		wdt_rst(0);
		//uart_print_dword(get_pos(&wr)); uart_write(' ');
	}
    ret = 0;
exit:
	return ret;
}

#pragma arm section
