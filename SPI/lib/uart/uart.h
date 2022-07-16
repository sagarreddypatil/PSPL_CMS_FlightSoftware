#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <avr/io.h>
#include "constants.h"

void uart_init();
void uart_putchar(char c, FILE *stream);
char uart_getchar(FILE *stream);

#endif