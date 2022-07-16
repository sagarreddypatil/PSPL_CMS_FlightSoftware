#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include "constants.h"

// Useful definitions to make stuff easier to read
#define SPI_DATA_REGISTER SPDR
#define SPI_INTERRUPT (SPSR & (1 << SPIF))

// Initialize SPI
void spi_init(uint8_t, uint8_t, uint8_t);

// Select an SPI slave
void spi_select(uint8_t);

// Send a command byte over the SPI bus
void spi_transmit(uint8_t);

// Read X bytes (up to 8) from the SPI bus
uint64_t spi_receive(uint8_t);

// Deselect an SPI slave
void spi_deselect(uint8_t);

#endif