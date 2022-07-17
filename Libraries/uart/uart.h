#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <avr/io.h>
#include <string.h>

#define BAUDRATE(N) ((F_CPU / (16 * N)) - 1)

void uart_init(uint32_t);
void uart_transmit(uint8_t);
uint8_t uart_receive();
void uart_printf(const char*, ...);

#endif