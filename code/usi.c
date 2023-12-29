/*
 * Kopiert von http://github.com/JChristensen/tinySPI
 * Arduino tinySPI Library Copyright (C) 2018 Jack Christensen GNU GPL v3.0
 * 
 * Verändert von gfl
 */
 
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
 
/*
 * Die Modi:
 * Mode | CPOL | CPHA
 *   0     0       0
 *   1     0       1
 *   2     1       0
 *   3     1       1
 * CPOL 0: auslesen bei steigender Flanke von Clock (Clock ist angehalten: low)
 * CPOL 1: auslesen bei fallender Flanke von Clock  (Clock ist angehalten: high) 
 * CPHA 0: auslesen bei erster Flanke von Clock
 * CHPA 1: auslesen bei zweiter Flanke von Clock
 * 
 * ATTiny25/45/85 kann nur Modus 0 und 1
 * 
 * DO_PIN    B1
 * USCK_PIN  B2
 * CS_PIN    B0
 * DI_PIN    B0	wird nicht benutzt
 */

#include "usi.h"
#include <util/atomic.h>

//Initializes the SPI bus, sets the USCK and DO pins to outputs, sets the DI pin to input
void initUSI() {
	USICR &= ~((1 << USISIE) | (1 << USIOIE) | (1 << USIWM1));
    USICR |=   (1 << USIWM0) | (1 << USICS1) | (1 << USICLK);
    
    setOutput(DO_PIN);
    setOutput(USCK_PIN);
    setOutput(CS_PIN);
    clrPin(DO_PIN);
    clrPin(USCK_PIN);
    
    setPin(CS_PIN);			//Abhängig von Usi-Mode
}

void setDataMode(uint8_t mode) {
    if (mode == USI_MODE1)
        USICR |= (1 << USICS0);
    else
        USICR &= ~(1 << USICS0);
}

void setCS_PIN() {
	setPin(CS_PIN);
}

void clrCS_PIN() {
	clrPin(CS_PIN);
}

//Transfers one byte over the USI bus, both sending and receiving. 
//Interrupts are inhibited during each byte transfer to ensure a consistent bit clock period.
uint8_t transferUsi(uint8_t data) {
    USIDR = data;
    USISR &= ~(1 << USIOIF);                // clear counter and counter overflow interrupt flag
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)   // ensure a consistent clock period
    {
        while ( !(USISR & (1 << USIOIF)) ) USICR |= (1 << USITC);
    }
    return USIDR;
}

//Disables the SPI bus
void endUSI() {
    USICR &= ~((1 << USIWM1) | (1 << USIWM0));
    setPin(USCK_PIN);
}
