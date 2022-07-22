#include "timer.h"

volatile uint64_t interrupt_count = 0;

// Interrupt handler for when the timer overflows
ISR(TIMER1_OVF_vect) {
    interrupt_count++;
}

// Initialize the microsecond timer, can also be used to reset the timer
void timer_init() {
    // 15.11.2: Temporarily disable Timer 0
    TCCR1B = 0x00;
    // 15.11.4: Set the timer to 0
    TCNT1 = 0x00;
    // 15.11.9: Clear the timer overflow flag
    TIFR1 = 0x00;
    // 15.11.8: Enable the timer overflow flag
    TIMSK1 = 0x01;
    // 15.11.1: Set timing to be normal
    TCCR1A = 0x00;
    // 15.11.2: Set the timer to tick once every 8 clocks
    TCCR1B = 0x02;
    // Enable global interrupts (if it hasn't been done already duh doi)
    sei();
}

/*
Return the time in microseconds since timer_init() was last called

Returns:
    uint64_t: time in microseconds since timer_init() was last called, resolution is 0.5uS
*/
uint64_t timer_micros() {
    return ((interrupt_count << 16) + TCNT1) >> 1;
}