/*
 * Kopiert von http://github.com/JChristensen/tinySPI
 * Arduino tinySPI Library Copyright (C) 2018 Jack Christensen GNU GPL v3.0
 * 
 * Verändert von gfl
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
 */

#include "usi.h"
#include <util/atomic.h>

//Initializes the SPI bus, sets the USCK and DO pins to outputs, sets the DI pin to input
void initUSI() {
	USICR &= ~(_BV(USISIE) | _BV(USIOIE) | _BV(USIWM1));
    USICR |= _BV(USIWM0) | _BV(USICS1) | _BV(USICLK);
    
    setOutput(DO_PIN);
    setOutput(USCK_PIN);
    setOutput(CS_PIN);
    clrPin(DO_PIN);
    clrPin(USCK_PIN);
    
    setPin(CS_PIN);			//Abhängig von Usi-Mode
}

void setDataMode(uint8_t mode) {
    if (mode == USI_MODE1)
        USICR |= _BV(USICS0);
    else
        USICR &= ~_BV(USICS0);
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
        while ( !(USISR & _BV(USIOIF)) ) USICR |= _BV(USITC);
    }
    return USIDR;
}

//Disables the SPI bus (leaving the pin modes unchanged)
void end() {
    USICR &= ~(_BV(USIWM1) | _BV(USIWM0));
}
