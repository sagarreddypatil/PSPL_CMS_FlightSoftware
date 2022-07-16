#ifndef UART_H
#define UART_H

#ifndef BAUD
#define BAUD 115200
#endif

#include <stdio.h>
#include <util/setbaud.h>
#include <avr/io.h>

void uart_init();
void uart_putchar(char c, FILE *stream);
char uart_getchar(FILE *stream);

#endif