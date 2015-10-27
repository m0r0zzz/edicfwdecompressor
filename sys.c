#include "edic_defs.h"
#include "sys.h"
#include "lzma.h"
#include "code.h"

#pragma arm section rodata = ".ARM.unlzma" code = ".ARM.unlzma" rwdata = ".ARM.unlzma" zidata = ".ARM.unlzma"    

//__global_reg(8) uint32_t reg0;
uint32_t timeToWdt;

__asm void long_jump_to_start(){
	
	mov r0, r10
	
	ldr r2, sysmem
	ldr r3, [r2]
	mov sp, r3
	ldr r3, [r2, #4]
	bx r3
	
	align
sysmem
		DCD SYSMEM_BASE
}

void wdt_rst(uint8_t force){
	uint32_t r, r2;
	if( (TIMER > timeToWdt) || force){
		r = TIMER;
		do { r2 = TIMER; }
		while(r2 == r);
		WDOG = ~r2;
		timeToWdt = TIMER + SYS_WDT_RST_PERIOD;
	}
}

void uart_write(uint32_t data){
	while(STATUS & _b(TX_BUSY));
	UART = data;
}

void _unlzma_veneer(){
	uart_write('D'); uart_write(' ');
	unlzma((uint8_t *)code, (uint8_t *)SYSMEM_BASE, 16384);
//	uart_write('S'); uart_write(' ');
	long_jump_to_start();
}
#pragma arm section
