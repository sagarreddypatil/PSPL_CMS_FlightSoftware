#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <util/delay.h>

// Found this to reverse the bits in a byte, O(1) time (very fast) http://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious
#define R2(n)     n,     n + 2*64,     n + 1*64,     n + 3*64
#define R4(n) R2(n), R2(n + 2*16), R2(n + 1*16), R2(n + 3*16)
#define R6(n) R4(n), R4(n + 2*4 ), R4(n + 1*4 ), R4(n + 3*4 )

void print_byte(uint8_t);

void sleep_ms(uint16_t);
void sleep_us(uint32_t);

#endif