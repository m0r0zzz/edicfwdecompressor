#include "edic_defs.h"
#include "sys.h"

__asm void tmp_mem_copy(uint32_t *src, uint32_t *dst, uint32_t len) {
	push {r3, lr}
	adds r2, r0, r2
	
transferloop
	ldmia r0!, {r3}
	stmia r1!, {r3}
//	ldmia r0!, {r3-r4}
//	stmia r1!, {r3-r4}
//	ldrh r3, [r0,#0]
//	strh r3, [r1,#0]
//	adds r0, #2
//	adds r1, #2
	cmp r2, r0
	bhi transferloop
	
	pop {r3, pc}
}

void scatter(){
	extern unsigned int Image$$_DATA$$Base, Image$$_DATA$$Length;
	extern unsigned int Load$$_DATA$$Base;
	extern unsigned int Image$$_CODE$$Base, Image$$_CODE$$Length;
	extern unsigned int Load$$_CODE$$Base;
	uint32_t *datadst = (uint32_t *)&Image$$_DATA$$Base;
	uint32_t *datasrc = (uint32_t *)&Load$$_DATA$$Base;
	uint32_t datalen = (uint32_t)&Image$$_DATA$$Length;
	uint32_t *codedst = (uint32_t *)&Image$$_CODE$$Base;
	uint32_t *codesrc = (uint32_t *)&Load$$_CODE$$Base;
	uint32_t codelen = (uint32_t)&Image$$_CODE$$Length;
	tmp_mem_copy(codesrc, codedst, codelen); 
	tmp_mem_copy(datasrc, datadst, datalen); 
}

int main(){
	scatter();
	wdt_rst(1);
//	uart_write('M');
	_unlzma_veneer();
	
	return 0;
}
