#include "common.h"

const uint8_t reverse_byte[256] = {
    R6(0), R6(2), R6(1), R6(3)
};

void print_byte(uint8_t byte) {
    byte = reverse_byte[byte];
    for(uint8_t bit_pos = sizeof(byte) *  8; bit_pos > 0; bit_pos--) {
        printf("%d", byte % 2);
        byte /= 2;
    }
    printf("\n");
}

void sleep_ms(uint16_t milliseconds) {
    _delay_ms(milliseconds);
}

void sleep_us(uint32_t microseconds) {
    _delay_us(microseconds);
}
