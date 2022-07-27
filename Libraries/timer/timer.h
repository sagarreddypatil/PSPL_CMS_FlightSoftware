#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>

void timer_init();
void timer_delay(uint32_t);
uint64_t timer_micros();

#endif