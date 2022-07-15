#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

// SPI pins
#define SPI_DDR DDRB
#define MOSI    PINB3
#define MISO    PINB4
#define SCK     PINB5

// SPI MSB or LSB
#define SPI_MSB 0
#define SPI_LSB 1

// SPI Modes
#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3

// SPI Speed
#define SPI_SPEED 128

// Useful definitions to make stuff easier to read
#define SPI_DATA_REGISTER SPDR
#define SPI_INTERRUPT (SPSR & (1 << SPIF))

// Initialize SPI
void spi_init(uint8_t, uint8_t, uint8_t);

// Select an SPI slave
void spi_select(uint8_t);

// Send a command byte over the SPI bus
void spi_transmit(uint8_t);

// Read a byte from the SPI bus
uint8_t spi_receive();

// Deselect an SPI slave
void spi_deselect(uint8_t);

#endif