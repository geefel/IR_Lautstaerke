#ifndef UART_ATTINY_H
#define UART_ATTINY_H

//#define TEST_ROUTINE

#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "pin.h"

#define RX_PIN PINDEF(B, 1)	//Muss eingestellt werden!!!!!!!!!!! PCINT1
#define TX_PIN PINDEF(B, 4)	//Muss eingestellt werden!!!!!!!!!!!

//ORCVAL = F_CPU / BAUD_ / PRESCALE
#define BAUD_ 9600	          						//Muss eingestellt werden!!!!!!!!!!!
#define ORCVAL 63         	  						//Muss eingestellt werden!!!!!!!!!!!
#define ORCVAL_HALB (ORCVAL - 40)					//MUSS eingestellt werden!!!!!!!!!!!

//~ #define INTERRUPT_TIMER0_ENABLE 	TIMSK |= (1 << OCIE0A)
//~ #define INTERRUPT_TIMER0_DISABLE 	TIMSK &= ~(1 << OCIE0A)
//~ #define INTERRUPT_TIMER1_ENABLE 	TIMSK |= (1 << OCIE1A)
//~ #define INTERRUPT_TIMER1_DISABLE 	TIMSK &= ~(1 << OCIE1A)
//~ #define INTERRUPT_INT0_ENABLE 		GIMSK |= (1 << INT0)
//~ #define INTERRUPT_INT0_DISABLE 		GIMSK &= ~(1 << INT0)
//~ #define INT0_RESET 								GIFR |= 1 << INTF0
//~ #define INTERRUPT_ALL_DISABLE 		SREG &= ~0x80
//~ #define INTERRUPT_ALL_ENABLE 			SREG |= 0x80
//~ #define TIMER0_STOP 							TCCR0B &= ~((1 << CS02) || (1 << CS01) || (1 << CS00))
//~ #define TIMER0_START 							TCCR0B |= (1 << CS00)				//Muss eingestellt werden!!!!!!!!!!! Prescale
//~ #define TIMER0_RESET 							TCNT0 = 0
#define INTERRUPT_PCINT_ENABLE 		GIMSK |= (1 << PCIE)
#define INTERRUPT_PCINT_DISABLE 	GIMSK &= ~(1 << PCIE)
#define PCINT_RESET 							GIFR |= 1<< PCIF
#define PCINT_PORT_SELECT					PCMSK = 1 << PCINT1					//Muss eingestellt werden!!!!!!!!!!! PCINTn

void initUartSoft(uint16_t baud);
void sendDat(uint8_t dat);

void clearNewData();
uint8_t isNewData();
uint8_t getNewData();
uint8_t ergNr;

#endif //UART_ATTINY_H
