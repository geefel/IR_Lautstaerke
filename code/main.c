/*
 *                       ATMEL ATTINY 25/45/85. Ich habe 45er
 *
 *                                     +-\/-+
 * PCINT5 !RESET dW         ADC0 PB5  1|    |8  VCC
 * PCINT3 XTAL1  CLKI !OC1B ADC3 PB3  2|    |7  PB2 SCK  USCK SCL ADC1 T0    INT0 PCINT2 
 * PCINT4 XTAL2  CLKO  OC1B ADC2 PB4  3|    |6  PB1 MISO DO       AIN1 OC0B  OC1A PCINT1 
 *                               GND  4|    |5  PB0 MOSI DI   SDA AIN0 OC0A !OC1A PCINT0 AREF
 *                                     +----+
 */

//#define IR_PIN PINDEF(B, 2)	//in ir.h
//#define RX_PIN PINDEF(B, 1)	//in uart_attiny.h
//#define TX_PIN PINDEF(B, 4)	//in uart_attiny.h

#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart_attiny.h"
#include "print.h"
#include "ir.h"
#include "pin.h"

int main(void) {	
	
union _erg {
	uint8_t ergAr[4];
	uint32_t erg32;
} dataMain;
	
	initUartSoft(9600);
	setupIR();
	resetRepeatData();
	clearData();
	sei();
	
	
	while(1) {
		if(getNewIR()) {
			dataMain.erg32 = getData();
			if ((dataMain.ergAr[0] == 0) && (dataMain.ergAr[1] == 255))
				//~ sendDat(dataMain.ergAr[0]);
				//~ sendDat(dataMain.ergAr[1]);
				sendDat('D');
				sendDat(dataMain.ergAr[2]);
				sendDat('\n');
				//~ sendDat(dataMain.ergAr[3]);
				//~ out("%d, %d\n", dataMain.ergAr[2], dataMain.ergAr[3]); 
			resetNewIR();
			clearData();	//löscht data in ir.c!!!
		}
		else if(getRepeatData()) {
			sendDat('R');
			sendDat(dataMain.ergAr[2]);
			sendDat('\n');
			resetRepeatData();
		}
		
		//schlafen();
		
	}
	
}


