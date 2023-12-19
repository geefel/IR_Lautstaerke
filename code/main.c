#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart_hard.h"
#include "ir_Samsung32.h"

int main(void) {	
	
union _erg {
	uint8_t ergAr[4];
	uint16_t ergParaVal[2];
	uint32_t erg32;
} dataMain;

enum paraVal {
	id        = 0x0707,
	input	  = 0x01FE,	//-[->]
	tv        = 0x02FD,
	volPlus   = 0x0708,
	volMinus  = 0x0BF4,
	muteOnOff = 0x0FF0
};

	setupIR();
	initUartHW(9600);
	
	resetRepeatData();
	dataMain.erg32 = 0;
	clearData();
	sei();
	
	while(1) {
		if(getNewIR()) {
			dataMain.erg32 = getData();
			if (dataMain.ergParaVal[0] == id) {
				switch (dataMain.ergParaVal[1]) {
					case id: ; break;
					case input: ; break;
					case tv: ; break;
					case volPlus: ; break;
					case volMinus: ; break;
					case muteOnOff: ; break;
					default: ; break;
				}
			} 
			//~ uart_Transmit_Hard(dataMain.ergAr[0]);
			//~ uart_Transmit_Hard(dataMain.ergAr[1]);
			//~ uart_Transmit_Hard(dataMain.ergAr[2]);
			//~ uart_Transmit_Hard(dataMain.ergAr[3]);
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


