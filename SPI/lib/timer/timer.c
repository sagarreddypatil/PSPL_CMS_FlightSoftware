#include "timer.h"

volatile unsigned long timer0_overflow_count = 0;
 
ISR(TIMER0_OVF_vect)
{
    timer0_overflow_count++;
}

void timer_init() {
    TCCR1B = (0 << CS12) | (0 << CS11) | (1 << CS10);
    TIMSK1 = (1 << TOIE1);
}

uint32_t micros() {
    uint32_t m;
    uint8_t oldSREG = SREG, t;
     
    cli();
    m = timer0_overflow_count;
    t = TCNT0;
 
    if ((TIFR0 & _BV(TOV0)) && (t < 255))
        m++;
 
    SREG = oldSREG;
     
    return ((m << 8) + t) * (16 / 16);
}