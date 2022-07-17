#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

uint32_t micros();

uint16_t delta_micros(uint16_t);

uint8_t get_timer_interrupt();

uint32_t get_time();

void timer_init();

#endif