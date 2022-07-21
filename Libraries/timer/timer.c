#include "timer.h"

volatile uint64_t interrupt_count = 0;

// Interrupt handler for when the timer overflows
ISR(TIMER0_OVF_vect) {
    interrupt_count++;
}

// Initialize the microsecond timer, can also be used to reset the timer
void timer_init() {
    // 13.8.2: Temporarily disable Timer 0
    TCCR0B = 0x00;
    // 13.8.3: Set the timer to 0
    TCNT0 = 0x00;
    // 13.8.7: Clear the timer overflow flag
    TIFR0 = 0x00;
    // 13.8.6: Enable the timer overflow flag
    TIMSK0 = 0x01;
    // 13.8.1: Set timing to be normal
    TCCR0A = 0x00;
    // 13.8.2: Set the timer to tick once every 64 clocks
    TCCR0B = 0x03;
    // Enable global interrupts (if it hasn't been done already duh doi)
    sei();
}

/*
Return the time in microseconds since timer_init() was last called

Returns:
    uint64_t: time in microseconds since timer_init() was last called, resolution is 4uS
*/
uint64_t timer_micros() {
    return ((interrupt_count << 8) + TCNT0) * (64 / CLOCK_CYCLES_PER_US);
}