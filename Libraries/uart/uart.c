#include "uart.h"

/*
Initialize UART

Args:
    uint32_t baud_rate: the baud rate that we want to run UART at, can be up to 2mbps
*/
void uart_init(uint32_t baudrate) {
    // 19.10.5 and 19.11: Set baud rate
    baudrate = BAUDRATE(baudrate / 2);
    UBRR0H = (baudrate >> 8);
    UBRR0L = baudrate;

    // 19.10.2: Enable 2X mode
    UCSR0A = (1 << U2X0);
    // 19.10.3: Enable reciever and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // 19.10.4: Set frame format, 8 bit data, 2 stop bits
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (1 << USBS0);
}

/*
Transmit a byte over UART

Args:
    uint8_t data_byte: the byte we want to send over UART (duh doi)
*/
void uart_transmit(uint8_t data_byte) {
    // 19.10.2: Wait for the data register to be empty
    while(!(UCSR0A & (1 << UDRE0)));
    // 19.10.1: Send the data
    UDR0 = data_byte;
}

/*
Receive a byte from UART

Returns:
    uint8_t: contents of the UART data register
*/
uint8_t uart_receive() {
    // 19.10.2: Wait for new data to come in
    while(!(UCSR0A & (1 << RXC0)));
    // 19.10.1: Return the new data
    return UDR0;
}

/*
Operates exactly like printf except outputs over UART
*/
int uart_printf(const char* str, ...) {
    // Variable argument list
    va_list valist;

    // Format the string using vspringf
    va_start(valist, str);
    char formatted_str[strlen(str) + 20];
    int ret = vsprintf(formatted_str, str, valist);
    va_end(valist);

    // Put the string out one character at a time
    uint8_t str_len = strlen(formatted_str);
    for(int str_idx = 0; str_idx < str_len; str_idx++) {
        uart_transmit(formatted_str[str_idx]);
    }

    return ret;
}