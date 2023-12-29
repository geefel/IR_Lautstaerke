#include "pin.h"

#define DO_PIN PINDEF(B, 1)
//#define DI_PIN PINDEF(B, 0)	
#define USCK_PIN  PINDEF(B, 2)
#define CS_PIN   PINDEF(B, 0)	//muss CS_PIN HIGH oder LOW? Abhängig von Usi-Mode

// SPI data modes
#define USI_MODE0 0x00
#define USI_MODE1 0x04

void initUSI();
void setDataMode(uint8_t mode);
void setCS_PIN();
void clrCS_PIN();
uint8_t transferUsi(uint8_t data);
void endUSI();
