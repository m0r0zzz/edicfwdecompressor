#ifndef SYS_H
#define SYS_H

#include <stdint.h>

//extern uint32_t reg0;

#define SYS_MS_DELAY_LOSC_CONST ((TIMER_FREQ + 500) / 1000)
#define SYS_WDT_RST_PERIOD SYS_MS_DELAY_LOSC_CONST*700

void mem_copy(uint32_t *src, uint32_t *dst, uint32_t len);

void long_jump_to_start(void);

void _unlzma_veneer(void);

void wdt_rst(uint8_t force);

void uart_write(uint32_t data);

void uart_print_byte(uint8_t data);
void uart_print_word(uint16_t data);
void uart_print_dword(uint32_t data);
void uart_print_bytes(uint8_t *data, uint32_t n);
	
#endif
