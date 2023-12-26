#include <util/delay.h>
#include <avr/interrupt.h>
#include "ir_Samsung32.h"
#include "usi.h"
#include "main.h"

#define AUDIO_SW_PIN PINDEF(B, 3)

union _erg {
	uint8_t ergAr[4];
	uint16_t ergParaVal[2];
	uint32_t erg32;
} dataMain;

enum paraVal {
	id        = 0x0707,
	input	    = 0x01FE,	//-[->]
	tv        = 0x02FD,
	volPlus   = 0x0708,
	volMinus  = 0x0BF4,
	muteOnOff = 0x0FF0
};

enum audioChanel {
	chanel1 = 1,
	chanel2 = 0
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

void test() {
	uint8_t maske1 = 1;
	dataMain.erg32 = 0x0f070301;
	for (int j = 0; j < 4; ++j) {
		for (int i = 0; i < 8; ++i) {
			if (dataMain.ergAr[j] & maske1) {
				clrPin(AUDIO_SW_PIN);
				_delay_ms(2);
				setPin(AUDIO_SW_PIN);
				_delay_ms(1);
			}
			else {
				clrPin(AUDIO_SW_PIN);
				_delay_ms(1);
				setPin(AUDIO_SW_PIN);
				_delay_ms(1);
			}
			maske1 <<= 1;
			
		}
		//~ clrPin(AUDIO_SW_PIN);
		//~ _delay_ms(4);
		setPin(AUDIO_SW_PIN);
		_delay_ms(4);
		maske1 = 1;
	}
	clrPin(AUDIO_SW_PIN);
	_delay_ms(1);
	setPin(AUDIO_SW_PIN);
	_delay_ms(1);
}

void setAudioIn(uint8_t aIn);
void switchAudioIn();
void setAudioLevelPlus();
void setAudioLevelMinus();
void setMute();
void sendAudioLevel(int16_t lvl);
uint8_t sendDataToUsi(uint8_t dat[2]);

//AUDIO_SW_PIN = HIGH: Chanel 1 ist an, Channel 2 ist aus
//AUDIO_SW_PIN = LOW : Chanel 2 ist an, Channel 1 ist aus
void setAudioIn(uint8_t aIn) {
	if (aIn == chanel1)
		setPin(AUDIO_SW_PIN);	
	else if (aIn == chanel2)
		clrPin(AUDIO_SW_PIN);
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
	uint8_t dat[2];
	dat[0] = writeData | selectPotAll;
	dat[1] = (uint8_t)(lvl & 255);
	sendDataToUsi(dat);	
}

uint8_t sendDataToUsi(uint8_t dat[2]) {
	clrCS_PIN();			//Abhängig von Usi-Mode
	transferUsi(dat[0]);
	transferUsi(dat[1]);
	setCS_PIN();			//Abhängig von Usi-Mode
	return 1;
}

int main(void) {
	setupIR();
	
	initUSI();
	setDataMode(USI_MODE0);	//Digital-Poti MCP41XXX/42XXX hat SPI-Mode 0
	
	setAudioIn(chanel1);
	
	resetRepeatData();
	dataMain.erg32 = 0;
	clearData();
	sei();
	
	while(1) {
		if(getNewIR()) {
			dataMain.erg32 = 0x0707;test();
			if (dataMain.erg32 == id) 
			if (dataMain.ergParaVal[0] == 0) {//test();
				switch (dataMain.ergParaVal[1]) {
					case input:     switchAudioIn();      break;
					case tv:                           ;  break;
					case volPlus:   setAudioLevelPlus();  break;
					case volMinus:  setAudioLevelMinus(); break;
					case muteOnOff: setMute();            break;
					default: test();  break;
				}
			} 
			resetNewIR();
			clearData();	//löscht data in ir.c!!!
		}
		
		//schlafen();
		
	}	
}


