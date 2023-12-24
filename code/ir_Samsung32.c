/*
 * Die Pulse bestehen aus hochfrequenten Rechtecken (38kHz, 1:4), die im Encoder mittels Bandpass in "ein" Rechteck gewandelt werden.
 * Die Nachricht beginnt mit einem AGC-Puls gefolgt von einem Zwischenraum.
 * Es wird Puls-Distance-Dekoding benutzt, d.h. ein Bit beginnt mit einem HIGH fester Länge und einer variablen Länge LOW,
 * je nachdem ob eine 1 oder eine 0 dargestellt wird. Am Ende der Nachricht wird ein weiter letzter Puls gesendet.
 * 
 * Es wird eine 16-bit Adresse und ein 16-bit Komando(Taste) übermittelt, beide LSB. Bei NEC sind die Komandos aber nur 8-bit lang, 
 * d.h. man kann die letzten 8 Bits ignorieren.
 * 
 * Protocol=Samsung32 Address=0x070 32 bits LSB first
 *	
 * Frequenz 	    38 kHz
 * Kodierung 	  	Pulse Distance
 * Frame 	      	1 Start-Bit + 32 Daten-Bits + 1 Stop-Bit
 * Daten NEC 	  	16 Adress-Bits + 16 Kommando-Bits
 * Bit-Order 	  	LSB first 
 * 
 * Bei fallender Flanke und ATmega, 16000000Hz, Prescale 1, OCR0A = 13, 112,38 µs
 * Start-bit: 4,5ms	
 * Pause:			4,5ms
 * =					9ms	Count 163
 * 0-bit: 		0,55ms
 * Pause:			0,55ms
 * =					1,1ms	Count 20
 * 1-bit: 		0,55ms	
 * Pause:			1,65ms
 * =					2,2ms	Count 40
 * stop: 			0,55ms
 * 
 * Wiederholung
 * nach 47 ms
 * 
 * ID    = 0x0707
 * Vol + = 0x0708
 * Vol - = 0x0BF4
 * TV    = 0x02FD
 * -[->] = 0x01FE
 * 
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include "ir_Samsung32.h"
#include "uart_hard.h"

#define ENDE_IR			0
#define BIT_IR 			1
#define BIT_START 	2

#define LAENGE_START 						163
#define LAENGE_WIEDERHOLUNG 		204
#define IF_START_WIEDERHOLUNG 	LAENGE_START + LAENGE_WIEDERHOLUNG / 2

#define LAENGE_BIT_1 		39
#define LAENGE_BIT_0 		20
#define IF_BIT_1_BIT_0 	30	//LAENGE_BIT_1 + LAENGE_BIT_0 / 2

static volatile uint16_t ti = 0;
static volatile uint8_t newIR = 0;
static volatile uint8_t repeatData = 0;

union {
	uint8_t ergAr[4];
	uint32_t erg32;
} data;

void setupTimer();
void setupInt0();
void startTimer();
void stopTimer();
void setNewIR();
void setRepeatData();
uint16_t getTime();

void setupIR() {
	setupInt0();
	setupTimer();
}

void setupInt0() {
	MCUCR |= 1 << ISC01;
	GIMSK |= 1 << INT0;
}

void setupTimer(){ 
	TCCR0A = 1 << WGM01;
	TIMSK |= 1 << OCIE0A;       // Enable output compare interrupt
    stopTimer();
    OCR0A = 13;                // 112,38 µs
}

void stopTimer() {
	TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
}

void startTimer() {
	TCCR0B = (1 << CS01) | (1 << CS00);
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
	static uint8_t t = 0;
	static uint8_t status = ENDE_IR;

	switch (status) {
		
		case ENDE_IR:
			startTimer();
			ti = 0;
			status = BIT_IR;
			break;
			
		case BIT_IR:			//erstes Datenbit beginnt hier
			t = ti;
			ti = 0;
			status = BIT_START;
			//~ if (t < IF_START_WIEDERHOLUNG)	{		//Wiederholung erkannt, Beginn Stopbit
				//~ status = ENDE_IR;
				//~ setRepeatData();
				//~ stopTimer();
			//~ }
			break;
									
		case BIT_START:							//zweites Bit beginnt hier
			t = ti;										//Auswertung des ersten Bit
			ti = 0;
			if (t > IF_BIT_1_BIT_0)		//40 für log 1 und 20 für log 0
				data.ergAr[bitB] |= maske;
			maske <<= 1;
			bitA++;
			if (bitA == 8) {					//data.ergAr[0,1,2] ist voll
				maske = 1;
				bitA = 0;
				bitB++;
			}
			if (bitB == 4) {				//data.ergAr[3] ist voll, Stopbit hat begonnen
				maske = 1;
				bitA = 0;
				bitB = 0;
				status = ENDE_IR;
				stopTimer();
				setNewIR();
			}
			break;
	}
}


