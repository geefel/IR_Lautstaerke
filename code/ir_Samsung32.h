#ifndef IR_H
#define IR_H

#include "pin.h"

#define IR_PIN PINDEF(B, 4)

void setupIR();
void resetNewIR();
void clearData();
uint32_t getData();
uint8_t getNewIR();
uint8_t getRepeatData();
void resetRepeatData();

#endif	//IR_H
