/*
   https://forum.arduino.cc/t/projekt-schnelle-digital-eingabe-ausgabe/358506/8
   Dank an combie für das Makro PINDEF(PORTNR,PINNR), die Art der Parameterliste und static- und inline-Sachen!

   Benutzung von pin.h:

   #include "pin.h"

//defines am besten in z.B. spi_soft.h oder uart_soft.h
   #define MOSI_PIN PINDEF(B, 0) //PORTB0
   #define MISO_PIN PINDEF(B, 1) //PORTB1	
   #define CLK_PIN  PINDEF(D, 7) //PORTD7
   #define CS_PIN   PINDEF(B, 3) //PORTB3

*/

#ifndef PIN_H
#define PIN_H

#include <stdint.h>
#include <avr/io.h>

static __attribute__((unused)) uint8_t maske_ = 1;

#define PINDEF(PORTNR, PINNR) &DDR##PORTNR, &PORT##PORTNR, &PIN##PORTNR, maske_<< PINNR

#define PARAMETERLISTE \
  volatile __attribute__((unused)) uint8_t *DDR, \
  volatile __attribute__((unused)) uint8_t *PORT, \
  volatile __attribute__((unused)) uint8_t *PIN,  \
  volatile __attribute__((unused)) uint8_t maske_

static inline uint8_t getPin(PARAMETERLISTE) {
  return (*PIN & maske_) ? 1 : 0;
}
static inline void setOutput(PARAMETERLISTE) {
  *DDR |= maske_;
}
static inline void setInput(PARAMETERLISTE) {
  *DDR &= ~maske_;
}
static inline void setInputPullup(PARAMETERLISTE) {
  *DDR &= ~maske_;
  *PORT |= maske_;
}
static inline void setTristate(PARAMETERLISTE) {
	*DDR &= ~maske_; 
	*PORT &= ~maske_; 
}
static inline void setPin(PARAMETERLISTE) {
  *PORT |= maske_;
}
static inline void clrPin(PARAMETERLISTE) {
  *PORT &= ~maske_;
}
static inline void togglePin(PARAMETERLISTE) {
  *PORT ^= maske_;
}

#endif  //PIN_H

