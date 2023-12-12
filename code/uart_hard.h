#ifndef UART_HARD_H
#define UART_HARD_H

#include <avr/io.h>

void initUartHW(uint32_t baud);
void uart_Transmit_Hard(uint8_t data);
uint8_t uart_Receive();
void free_uart_pins();

#endif	//UART_HARD_H
