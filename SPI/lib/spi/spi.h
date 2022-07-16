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

// SPI Clock Speeds
#define SPI_SPEED0 0
#define SPI_SPEED1 1
#define SPI_SPEED2 2
#define SPI_SPEED3 3
#define SPI_SPEED4 4
#define SPI_SPEED5 5
#define SPI_SPEED6 6
#define SPI_SPEED7 7

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