#include "uart.h"

void uart_init(uint32_t baud_rate) {
    // 19.10.5 and 19.11: Set baud rate
    baud_rate = BAUDRATE(baud_rate / 2);
    UBRR0H = (baud_rate >> 8);
    UBRR0L = baud_rate;

    // 19.10.2: Enable 2X mode
    UCSR0A = (1 << U2X0);
    // 19.10.3: Enable reciever and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // 19.10.4: Set frame format, 8 bit data, 2 stop bits
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (1 << USBS0);
}

void uart_transmit(uint8_t data_byte) {
    // 19.10.2: Wait for the data register to be empty
    while(!(UCSR0A & (1 << UDRE0)));
    // 19.10.1: Send the data
    UDR0 = data_byte;
}

uint8_t uart_receive() {
    // 19.10.2: Wait for new data to come in
    while(!(UCSR0A & (1 << RXC0)));
    // 19.10.1: Return the new data
    return UDR0;
}

void uart_printf(const char* str, ...) {
    va_list valist;

    // Format the string using vspringf
    va_start(valist, str);
    char formatted_str[strlen(str) + 20];
    vsprintf(formatted_str, str, valist);
    va_end(valist);

    // Put the string out one character at a time
    uint8_t str_len = strlen(formatted_str);
    for(int str_idx = 0; str_idx < str_len; str_idx++) {
        uart_transmit(formatted_str[str_idx]);
    }
}