#include "spi.h"

// http://www.rjhcoding.com/avrc-spi.php

void spi_init(uint8_t data_order, uint8_t spi_mode, uint8_t sck_speed) {
    SPI_DDR = ((1 << DDB2) | (1 << DDB1) | (1 << DDB0)); //spi pins on port b MOSI SCK,SS outputs
    SPCR = ((1 << SPE) | (1 << MSTR));  // SPI enable, Master, SPI mode 0, clock speed = F_CLK / 4

    // Set the data register to either read MSB first (0) or LSB first (1)
    SPCR |= (data_order << DORD);

    // Set the SPCR register according to what SPI mode the user has requested
    SPCR |= (((spi_mode & 1) << CPHA) | (((spi_mode & 2) >> 1) << CPOL));

    // Set the SPI clock speed, TODO: there's for sure a cleaner way to do this
    uint8_t spi_double_speed = ((((sck_speed & 2) >> 1) << SPI2X) | (((sck_speed & 8) >> 3) << SPI2X) | (((sck_speed & 32) >> 5) << SPI2X));
    if(spi_double_speed) {
        SPSR = (spi_double_speed << SPI2X);
        sck_speed *= 2;
    }
    if(sck_speed >= 64) {
        SPCR |= (1 << SPR1);
    }
    if(sck_speed == 16 || sck_speed == 128) {
        SPCR |= (1 << SPR0);
    }
}

void spi_transmit(uint8_t data) {
    SPI_DATA_REGISTER = data; // Slap our byte into the SPI register, this bad boy can hold 8 bits!
    while(!SPI_INTERRUPT); // Wait for transmission complete by looking at the SPI interrupt
}

uint8_t spi_receive() {
    SPI_DATA_REGISTER = 0x00; // Transmit zero (could be anything)
    while(!SPI_INTERRUPT); // Wait for the transmission to be complete by looking at the SPI interrupt
    return SPI_DATA_REGISTER; // Slap the byte our slave gave us into the SPI register, you get the rest
}

void spi_select(uint8_t slave_select) {
    SPI_DDR &= ~(1 << slave_select); // Toggle the requested slave select low
}

void spi_deselct(uint8_t slave_select) {
    SPI_DDR |= (1 << slave_select); // Toggle the requested slave select high
}
