#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart_hard.h"
#include "ir_Samsung32.h"

int main(void) {	
	
union _erg {
	uint8_t ergAr[4];
	uint32_t erg32;
} dataMain;

	setupIR();
	initUartHW(9600);
	
	resetRepeatData();
	dataMain.erg32 = 0;
	clearData();
	sei();
	
	while(1) {
		if(getNewIR()) {
			dataMain.erg32 = getData();
			uart_Transmit_Hard(dataMain.ergAr[0]);
			uart_Transmit_Hard(dataMain.ergAr[1]);
			uart_Transmit_Hard(dataMain.ergAr[2]);
			uart_Transmit_Hard(dataMain.ergAr[3]);
			resetNewIR();
			clearData();	//löscht data in ir.c!!!
		}
		//~ else if(getRepeatData()) {
			//~ //uart_Transmit_Hard(dataMain.ergAr[2]);
			//~ resetRepeatData();
		//~ }
		
		//schlafen();
		
	}
	
}


