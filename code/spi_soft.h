#ifndef SPI_SOFT_H
#define SPI_SOFT_H

#include "pin.h"
#include <avr/io.h>

#define SCK_PIN	PINDEF(B, 0)
#define DO_PIN 	PINDEF(B, 1)
#define CS_PIN 	PINDEF(B, 4)

void initSPI();
uint8_t sendDat(uint8_t dat);
uint8_t sendData8(uint8_t dat);
uint16_t sendData16(uint16_t dat);

#endif  //SPI_SOFT_H
