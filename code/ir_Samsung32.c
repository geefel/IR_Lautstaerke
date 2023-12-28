/*
 * Die Pulse bestehen aus hochfrequenten Rechtecken (38kHz, 1:4), die im Encoder mittels Bandpass in "ein" Rechteck gewandelt werden.
 * Die Nachricht beginnt mit einem AGC-Puls gefolgt von einem Zwischenraum.
 * Es wird Puls-Distance-Dekoding benutzt, d.h. ein Bit beginnt mit einem HIGH fester Länge und einer variablen Länge LOW,
 * je nachdem ob eine 1 oder eine 0 dargestellt wird. Am Ende der Nachricht wird ein weiter letzter Puls gesendet.
 * 
 * Es wird eine 16-bit Adresse und ein 16-bit Komando(Taste) übermittelt, beide LSB. 
 * 
 * Protocol=Samsung32 Address=0x0707 32 bits LSB first
 *	
 * Frequenz 	    38 kHz
 * Kodierung 	  	Pulse Distance
 * Frame 	      	1 Start-Bit + 32 Daten-Bits + 1 Stop-Bit
 * Daten NEC 	  	16 Adress-Bits + 16 Kommando-Bits
 * Bit-Order 	  	LSB first 
 * 
 * Bei fallender Flanke und ATtiny, 1000000Hz, Prescale 1/1, OCR0A = 12, 0,012 ms
 * 
 * Start-bit:	4,5ms	
 * Pause:		4,5ms
 * =			9ms		Count 750
 * 
 * 0-bit: 		0,55ms
 * Pause:		0,55ms
 * =			1,1ms	Count 91,7
 * 
 * 1-bit: 		0,55ms	
 * Pause:		1,65ms
 * =			2,2ms	Count 183,3
 * 
 * stop: 		0,55ms	Count 45,8
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
#include "main.h"

const uint16_t start_laenge = 750;
const uint16_t bit1_laenge = 183;
const uint16_t bit0_laenge = 92;


volatile uint16_t ti;
volatile uint16_t t;
volatile uint8_t maske;
volatile uint8_t bitCount;
volatile uint8_t byteCount;

volatile uint8_t newIR;
volatile uint8_t repeatData;
volatile uint8_t status;

void setupTimer();
void setupInt0();

uint8_t isTimerRunning();
void setNewIR();
void setRepeatData();
uint16_t getTime();

void setupIR() {
	ti = 0;
	newIR = 0;
	repeatData = 0;
	maske = 1;
	bitCount = 0;
	byteCount = 0;
	t = 0;
	
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
    OCR0A = 12;
}

void stopTimer() {
	TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
}

void startTimer() {
	TCCR0B = (1 << CS00);
}

uint8_t isTimerRunning() {
	if (TCCR0B & ((1 << CS02) | (1 << CS01) | (1 << CS00)))
		return 1;
	else
		return 0;
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

ISR(INT0_vect) {
	t = ti;
	ti = 0;
	
	if (t > 100) {			//IR-Start ist geschehen
		bitCount = 0;
		byteCount = 0;
		maske = 1;
	}
	else if (t > 40) {		//Logisch 1 ist geschehen
		data_IR[byteCount] |= maske;
		maske <<= 1;
		bitCount++;
	}
	else if (t < 30) {		//Logisch 0 ist geschehen
		maske <<= 1;
		bitCount++;
	}
	if (bitCount == 8) {	//nächstes Byte soll gefüllt werden	
		bitCount = 0;
		byteCount++;
		maske = 1;
		if (byteCount == 4) {//viertes Byte ist gefüllt
			setNewIR();
		}
	}
}

