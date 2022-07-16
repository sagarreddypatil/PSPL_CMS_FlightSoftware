#include "uart.h"

// https://github.com/tuupola/avr_demo/tree/master/blog/simple_usart
// https://www.appelsiini.net/2011/simple-usart-with-avr-libc/

void uart_init() {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    
    UCSR0A |= (USE_2X << U2X0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Transmit 8 bit data
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);   // Enable RX and TX
}

void uart_putchar(char c, FILE *stream) {
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

char uart_getchar(FILE *stream) {
    while(!(UCSR0A & (1 << RXC0)));
    return UDR0;
}