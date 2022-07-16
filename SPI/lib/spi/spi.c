#include "spi.h"

void spi_init(uint8_t data_order, uint8_t spi_mode, uint8_t sck_speed) {
    DDRB = ((1 << MOSI) | (1 << SCK) | (1 << SS)); // SPI output pins on port B: MOSI, SCK and Master SS

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

uint8_t spi_receive() {
    SPI_DATA_REGISTER = 0xBC; // Transmit CA for Big Cameron, doesn't really matter what's here. If you change it to 0x69 I will personally end you
    while(!SPI_INTERRUPT); // Wait for the transmission to be complete by looking at the SPI interrupt
    return SPI_DATA_REGISTER;
}

void spi_deselect(uint8_t slave_select) {
    PORTB &= ~(1 << slave_select); // Toggle the requested slave select low (active low)
}

void spi_select(uint8_t slave_select) {
    PORTB |= (1 << slave_select); // Toggle the requested slave select high
}