#include <stdio.h>
#include <stdint.h>
#include "spi.h"

extern uint8_t reverse_byte[];

void init_spi() {
    DDRB = (1 << SCK) | (1 << CS) | (1 << MOSI);
}

void command_spi(uint8_t command_byte) {
    // Reverse the bits in the byte (0001 --> 1000)
    command_byte = reverse_byte[command_byte];

    // Send the command_byte over SPI starting from most significant bit (MSB)
    for(uint8_t bit_pos = 7; bit_pos >= 0; bit_pos--) {
        // Set MOSI
        PORTB = (command_byte % 2) << MOSI;
        print_byte(PORTB);
        // Set MOSI and Clock
        PORTB |= 1 << SCK;
        print_byte(PORTB);
        command_byte /= 2;
    }
}

uint8_t read_spi() {
    uint8_t read_byte = 0;
    for(uint8_t bit_pos = 7; bit_pos >= 0; bit_pos--) {
        // Set all to 0
        PORTB = 0;
        print_byte(PORTB);
        // Set Clock
        PORTB |= 1 << SCK;
        print_byte(PORTB);
        read_byte += (PINB >> MISO) << bit_pos;
    }
}
