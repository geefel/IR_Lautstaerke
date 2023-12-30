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


#include <util/delay.h>
#include <avr/interrupt.h>
#include "ir_Samsung32.h"
#include "usi.h"
#include "main.h"

#define AUDIO_SW_PIN PINDEF(B, 3)

const uint16_t id 		 = 0x0707;

const uint16_t volPlus   = 0xF807;
const uint16_t volMinus  = 0xF40B;
const uint16_t muteOnOff = 0xF00F;
const uint16_t input	 = 0xFE01;	//-[->]
const uint16_t tv        = 0xFD02;

enum audioChanel {
	chanel2 = 0,
	chanel1 = 1
};

enum mute {
	isNotMute = 0,
	isMute = 1
};

enum levelMaxMin {
	audioLevelMin = 0,
	audioLevelMax = 255
};

enum MCP42010_Command {
	noCommand1   = 0b00000000,
	writeData    = 0b00010000,
	shutdown     = 0b00100000,
	noCommand2   = 0b00110000,
	selectNon    = 0b00000000,
	selectPot0   = 0b00000001,
	selectPot1   = 0b00000010,
	selectPotAll = 0b00000011
};

const uint8_t audioLevelStep = 1;

uint8_t muteStatus = isMute;
uint8_t audioInChanel = chanel1;
int16_t audioLevel = audioLevelMin;

void setAudioIn(uint8_t aIn);
void switchAudioIn();
void setAudioLevelPlus();
void setAudioLevelMinus();
void setMute();
void sendAudioLevel(int16_t lvl);
void sendDataToUsi(uint8_t dat[2]);
void clearIRData();

void test() {
	uint8_t maske1 = 1;
	clrPin(AUDIO_SW_PIN);
	for (int j = 0; j < 4; ++j) {
		for (int i = 0; i < 8; ++i) {
			if (data_IR[j] & maske1) {
				setPin(AUDIO_SW_PIN);
				_delay_us(90);
				clrPin(AUDIO_SW_PIN);
				_delay_us(10);
			}
			else {
				setPin(AUDIO_SW_PIN);
				_delay_us(10);
				clrPin(AUDIO_SW_PIN);
				_delay_us(90);
			}
			maske1 <<= 1;
		}
		maske1 = 1;
	}
	clrPin(AUDIO_SW_PIN);
}
void te(uint16_t erg) {
	uint16_t maske = 0x8000;
	clrPin(AUDIO_SW_PIN);
	for (int i = 16; i > 0; --i) {
		if (erg & maske)
			te1();
		else
			te0();
		maske >>= 1;
	}
	clrPin(AUDIO_SW_PIN);
}
void te1() {
	setPin(AUDIO_SW_PIN);
	_delay_us(90);
	clrPin(AUDIO_SW_PIN);
	_delay_us(10);
}
void te0() {
	setPin(AUDIO_SW_PIN);
	_delay_us(10);
	clrPin(AUDIO_SW_PIN);
	_delay_us(90);
}

//AUDIO_SW_PIN = HIGH: Chanel 1 ist an, Channel 2 ist aus
//AUDIO_SW_PIN = LOW : Chanel 2 ist an, Channel 1 ist aus
void setAudioIn(uint8_t aIn) {
	if (aIn == chanel1)
		setPin(AUDIO_SW_PIN);	
	else if (aIn == chanel2)
		clrPin(AUDIO_SW_PIN);
	//Die Fernbedienung sendet innerhalb von 325ms 0xFE01 vier Mal, deshalb:
	_delay_ms(350);
}

void switchAudioIn() {
	audioInChanel = audioInChanel == chanel1 ? chanel2 : chanel1;
	setAudioIn(audioInChanel);
}

void setAudioLevelPlus() {
	audioLevel += audioLevelStep;
	if (audioLevel > audioLevelMax)
		audioLevel = audioLevelMax;
	sendAudioLevel(audioLevel);
}

void setAudioLevelMinus() {
	audioLevel -= audioLevelStep;
	if (audioLevel < audioLevelMin) 
		audioLevel = audioLevelMin;
	sendAudioLevel(audioLevel);
}

void setMute() {
	muteStatus = muteStatus == isNotMute ? isMute : isNotMute;
	if (muteStatus == isNotMute)
		sendAudioLevel(audioLevel);
	else if (muteStatus == isMute)
		sendAudioLevel(audioLevelMin);
}




void sendAudioLevel(int16_t lvl) {
	initUSI();
	setDataMode(USI_MODE0);
	clrCS_PIN();
	transferUsi(writeData | selectPotAll);
	setCS_PIN();
	
initUSI();
	setDataMode(USI_MODE0);

	clrCS_PIN();
	transferUsi(0b10101010);
	setCS_PIN();
	endUSI();
	
	//~ uint8_t dat[2];
	//~ dat[0] = writeData | selectPotAll;
	//~ dat[1] = (uint8_t)(lvl & 255);
	//~ sendDataToUsi(dat);	
}

//~ void sendDataToUsi(uint8_t dat[2]) {
	//~ initUSI();
	//~ setDataMode(USI_MODE0);	//Digital-Poti MCP41XXX/42XXX hat SPI-Mode 0
	//~ clrCS_PIN();			//Abhängig von Usi-Mode
	//~ transferUsi(dat[0]);
	//~ transferUsi(dat[1]);
	//~ setCS_PIN();			//Abhängig von Usi-Mode
	//~ endUSI();
//~ }

void clearIRData() {
	for (int i = 0; i < 4; ++i)
		data_IR[i] = 0;
}

int main(void) {
	uint16_t data_id = 0;
	uint16_t data_val = 0;

	
	//initUSI();
	//setDataMode(USI_MODE0);	//Digital-Poti MCP41XXX/42XXX hat SPI-Mode 0
	
	setOutput(AUDIO_SW_PIN);
	setAudioIn(chanel1);
	
	setupIR();
	startTimer();
	
	sei();
	while(1) {
		if(getNewIR()) {
			data_id  = (data_IR[1] << 8) | data_IR[0];
			data_val = (data_IR[3] << 8) | data_IR[2];
			if (data_id == id) {
				stopIR();
				
				if (data_val == volPlus) {
					setAudioLevelPlus();
				}
				else if (data_val == volMinus) {
					setAudioLevelMinus();
				}
				else if (data_val == muteOnOff) {
					setMute();
				}
				else if (data_val == input) {
					switchAudioIn();
				}
				else if (data_val == tv) {
					;
				}
				else {
					;
				}				
			} 
			resetNewIR();
			clearIRData();
			setupIR();
		}
		
		//schlafen();
		
	}	
}


