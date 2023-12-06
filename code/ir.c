/*
 * Die Pulse bestehen aus hochfrequenten Rechtecken (38kHz, 1:4), die im Encoder mittels Bandpass in "ein" Rechteck gewandelt werden.
 * Die Nachricht beginnt mit einem AGC-Puls gefolgt von einem Zwischenraum.
 * Es wird Puls-Distance-Dekoding benutzt, d.h. ein Bit beginnt mit einem HIGH fester Länge und einer variablen Länge LOW,
 * je nachdem ob eine 1 oder eine 0 dargestellt wird. Am Ende der Nachricht wird ein weiter letzter Puls gesendet.
 * 
 * Es wird eine 16-bit Adresse und ein 16-bit Komando(Taste) übermittelt, beide LSB. Bei NEC sind die Komandos aber nur 8-bit lang, 
 * d.h. man kann die letzten 8 Bits ignorieren.
 * 
 * Protocol=NEC upd6122-chip Address=0x0 32 bits LSB first
 *	
 * Frequenz 	    36 kHz / 38 kHz(!)
 * Kodierung 	  	Pulse Distance
 * Frame 	      	1 Start-Bit + 32 Daten-Bits + 1 Stop-Bit
 * Daten NEC 	  	8 Adress-Bits + 8 invertierte Adress-Bits + 8 Kommando-Bits + 8 invertierte Kommando-Bits
 * Daten ext.NEC 	16 Adress-Bits + 8 Kommando-Bits + 8 invertierte Kommando-Bits
 * Bit-Order 	  	LSB first 
 * 
 * Bei fallender Flanke und ATmega, 1000000Hz, Prescale 1, OSC = 115
 * Start-bit: 9ms	
 * Pause:			4,5ms
 * =					13,5ms	Count 119
 * 0-bit: 		0,5625ms
 * Pause:			0,5625ms
 * =					1,125ms	Count 10
 * 1-bit: 		0,5625ms	
 * Pause:			1,6875ms
 * =					2,25ms	Count 20
 * stop: 			0,5625ms
 * 
 * Wiederholung
 * Start-bit: 9ms
 * Pause:			2,25ms
 * =					11,25ms		Count 99
 * Stop: 			0,5625ms
 * 
 * 
 * IR-Empfänger mit internem TiefPass an INT0-Pin PB2
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include "ir.h"
#include "print.h"
#include "uart_attiny.h"

#define ENDE_IR			0
#define BIT_IR 			1
#define BIT_START 	2

#define LAENGE_START 						119
#define LAENGE_WIEDERHOLUNG 		99
#define IF_START_WIEDERHOLUNG 	109	//LAENGE_START + LAENGE_WIEDERHOLUNG / 2

#define LAENGE_BIT_1 		20
#define LAENGE_BIT_0 		10
#define IF_BIT_1_BIT_0 	15 //LAENGE_BIT_1 + LAENGE_BIT_0 / 2

static volatile uint16_t ti = 0;
static volatile uint8_t newIR = 0;
static volatile uint8_t repeatData = 0;

union {
	uint8_t ergAr[4];
	uint32_t erg32;
} data;

void setupTimer0();
void setupInt0();
void startTimer0();
void stopTimer0();
void setNewIR();
void setRepeatData();
uint16_t getTime();

void setupIR() {
	setInputPullup(IR_PIN);
	setupInt0();
	setupTimer0();
}

void setupInt0() {
	MCUCR |= 1 << ISC01;
	GIMSK |= 1 << INT0;
}

void setupTimer0() { 
	TCCR0A = 1 << WGM01;
	TIMSK |= 1 << OCIE0A;  // Enable output compare interrupt
  stopTimer0();
  OCR0A = 115;
}

void stopTimer0() {
	TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
}

void startTimer0() {
	
	TCCR0B = (1 << CS00);
}

void clearData() {
	data.erg32 = 0;
}

uint32_t getData() {
	return data.erg32;
}

uint8_t getNewIR() {
	return newIR;
}

void resetNewIR() {
	newIR = 0;
}

void setNewIR() {
	newIR = 1;
}

void setRepeatData() {
	repeatData = 1;
}

void resetRepeatData() {
	repeatData = 0;
}

uint8_t getRepeatData() {
	return repeatData;
}

ISR(TIMER0_COMPA_vect){
	ti++;
}

ISR(INT0_vect){
	static uint8_t maske = 1;
	static uint8_t bitA = 0;
	static uint8_t bitB = 0;
	static uint16_t t = 0;
	static uint8_t status = ENDE_IR;
	switch (status) {
		
		case ENDE_IR:
			startTimer0();
			ti = 0;
			status = BIT_IR;
			break;
			
		case BIT_IR:				//erstes Datenbit beginnt hier
			t = ti;
			ti = 0;
			status = BIT_START;
			if (t < IF_START_WIEDERHOLUNG)	{		//Wiederholung erkannt, Beginn Stopbit
				status = ENDE_IR;
				setRepeatData();
				stopTimer0();
			}
			break;
									
		case BIT_START:			//zweites Bit beginnt hier
			t = ti;						//Auswertung des ersten Bit
			ti = 0;
			if (t > IF_BIT_1_BIT_0)
				data.ergAr[bitB] |= maske;
			maske <<= 1;
			bitA++;
			if (bitA == 8) {	//data.ergAr[0,1,2] ist voll
				maske = 1;
				bitA = 0;
				bitB++;
			}
			if (bitB == 4) {	//data.ergAr[3] ist voll, Stopbit hat begonnen
				maske = 1;
				bitA = 0;
				bitB = 0;
				status = ENDE_IR;
				stopTimer0();
				setNewIR();
			}
			break;
	}
}


