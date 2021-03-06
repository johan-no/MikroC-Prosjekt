/*
 * SETUP_usart.h
 * Author: olemarh
 * 
 */ 


#ifndef SETUP_USART_H_
#define SETUP_USART_H_

#define BAUDRATE 9600
#define UBRR_VALUE ((F_CPU / (BAUDRATE * 16UL)) - 1)


void usart_initial_startup(void)
{
	UBRR0 = UBRR_VALUE;
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0); /* Enable RX and TX */
}

void usart_transmit(uint8_t data)
{
	// Wait for empty transmit buffer
	//while( (!(UCSR0A & (1 << UDRE0))))
	UDR0 = data;
}

int uart_putchar(char c, FILE *stream)
{
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}




#endif /* SETUP_USART_H_ */