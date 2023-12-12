#include "uart_hard.h"

void initUartHW(uint32_t baud) {
	uint16_t ubrr = (uint16_t)(F_CPU/16.0/baud-0.5);
	/* Baudrate setzen */
  UBRR0 = ubrr;
  /* Sender und Empfänger freigeben */
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  /* Rahmenformat setzen: 8 Datenbits, 1 Stoppbits, keine Parität */
  UCSR0C = (3<<UCSZ00);
}

void uart_Transmit_Hard(uint8_t data) {
  /* Warte bis Sendepuffer frei */
  while (!(UCSR0A & (1<<UDRE0))) ;
  /* Put data into buffer, sends the data */
  UDR0 = data;
}

void free_uart_pins() {
	UCSR0B &= ~((1<<RXEN0)|(1<<TXEN0));
}

uint8_t uart_Receive() {
  /* Wait for data to be received */
  while (!(UCSR0A & (1<<RXC0)));
  /* Get and return received data from buffer */
  return UDR0;
}
