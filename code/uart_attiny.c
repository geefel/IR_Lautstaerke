#include "uart_attiny.h"
#include "util/delay.h"

uint8_t dataReceived;
static volatile uint8_t newData;
static volatile uint16_t maskeSend;
static volatile uint16_t maskeReceive;
volatile uint8_t isTimer1_Interrupt;
volatile uint8_t isPCINT_Interrupt;

//==============================TIMER1=================================
void setupTimer1();
void startTimer1();
void stopTimer1();
void clearTimer1();
void setOCR();
void setOCRshort();

void setupTimer1(){ 
	TCCR1 = 0;
	TIMSK |= 1 << OCIE1A;			//Interruptfreigabe
  OCR1A = ORCVAL;						//uart_attiny.h
}

void setOCR() {
	OCR1A = ORCVAL;
}

void setOCRshort() {
	OCR1A = ORCVAL_HALB;
}

void clearTimer1() {
	TCNT1 = 0;
}

void stopTimer1() {
	TCCR1 = 0;
}

void startTimer1() {
	TCCR1 = (1 << CS10);
}

//=====================================================================

void initUartSoft(uint16_t baud) {
	baud *= 1;
	maskeReceive = 1;
	maskeSend = 1;
	setupTimer1();

  setOutput(TX_PIN);
  setInputPullup(RX_PIN);
  setPin(TX_PIN);
  
	PCINT_PORT_SELECT;	//PCINT von B4 auslösen
	//External PCINT0 interrupt
  //MCUCR = (1 << ISC01);    // Interrupt sense control: Eine fallende Flanke am INT0-Pin löst Interrupt aus 

	clearNewData();
	
	INTERRUPT_PCINT_ENABLE; 	//in main.c?
}

//Es wird TIMER1 an/aus gemacht, sonst keiner (auch global)
void sendDat(uint8_t dat) {
	uint16_t d = (dat << 1) + 512;
	stopTimer1();
	clearTimer1();
	startTimer1();

	for (uint8_t i = 0; i < 10; i++) {	
		if (d & maskeSend) 
			setPin(TX_PIN);
		else
			clrPin(TX_PIN);
		maskeSend <<= 1;
		isTimer1_Interrupt = 0;
		while (!isTimer1_Interrupt) {asm volatile ("nop" ::);}
		clearTimer1();		// !!!!!!!
	}
  stopTimer1();
  maskeSend = 1;
}

uint8_t isNewData() {
	return newData;
}
void clearNewData() {
	newData = 0;
	dataReceived = 0;
}
uint8_t getNewData() {
	return dataReceived;
}


//Timer1 interrupt service routine.
ISR(TIMER1_COMPA_vect) {
	isTimer1_Interrupt = 1;
}

//PCINT0 interrupt service routine.
ISR(PCINT0_vect) {
	INTERRUPT_PCINT_DISABLE;
	clearTimer1();
	startTimer1();
	
	for (uint8_t i = 0; i < 9; ++i) {
		isTimer1_Interrupt = 0;
		while (!isTimer1_Interrupt) {asm volatile ("nop" ::);}
		if(getPin(RX_PIN))
			dataReceived |= maskeReceive;
		maskeReceive <<= 1;
	}
	
	newData = 1;
	isPCINT_Interrupt = 0;
	maskeReceive = 1;
	PCINT_RESET;		//Interrupt-Flag löschen
	INTERRUPT_PCINT_ENABLE;
}
