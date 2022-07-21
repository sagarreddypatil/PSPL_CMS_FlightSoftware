#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>

#define CLOCK_CYCLES_PER_US 16

void timer_init();
uint64_t timer_micros();

#endif