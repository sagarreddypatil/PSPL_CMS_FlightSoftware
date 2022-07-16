#include "spi.h"

// http://www.rjhcoding.com/avrc-spi.php

void spi_init(uint8_t data_order, uint8_t spi_mode, uint8_t sck_speed) {
    SPI_DDR = ((1 << MOSI) | (1 << SCK)); //spi output pins on port B, MOSI and SCK

    // Configure the SPI bus to be whatever the user requested
    // atmega328p datasheet page 140 and around there are useful for why we're doing these bit shifts
    SPCR = (
        (1 << SPE) |
        (1 << MSTR) |

        (data_order << DORD) |

        ((spi_mode & 1) << CPHA) |
        (((spi_mode & 2) >> 1) << CPOL) |

        (((sck_speed & 4) >> 2) << SPI2X) |
        (((sck_speed & 2) >> 1) << SPR1) |
        ((sck_speed & 1) << SPR0)
    );
}

void spi_transmit(uint8_t data) {
    SPI_DATA_REGISTER = data; // Slap our byte into the SPI register, this bad boy can hold 8 bits!
    while(!SPI_INTERRUPT); // Wait for transmission complete by looking at the SPI interrupt
}

uint64_t spi_receive(uint8_t num_bytes) {
    SPI_DATA_REGISTER = 0x00; // Transmit 0, doesn't really matter what's here. If you change it to 0x69 I will personally end you
    uint64_t data = 0;
    for(uint8_t byte = num_bytes; byte > 0; byte--) {
        while(!SPI_INTERRUPT); // Wait for the transmission to be complete by looking at the SPI interrupt
        data |= SPI_DATA_REGISTER << (byte * 8);
    }
    return data;
}

void spi_select(uint8_t slave_select) {
    SPI_DDR &= ~(1 << slave_select); // Toggle the requested slave select low
}

void spi_deselct(uint8_t slave_select) {
    SPI_DDR |= (1 << slave_select); // Toggle the requested slave select high
}
