#ifndef IR_H
#define IR_H

#include "pin.h"

#define IR_PIN PINDEF(B, 2)

void setupIR();
void stopIR();
void startTimer();
void stopTimer();
void resetNewIR();
uint8_t getNewIR();

#endif	//IR_H
