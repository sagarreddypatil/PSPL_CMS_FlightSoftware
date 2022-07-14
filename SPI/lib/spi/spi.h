#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include "common.h"

// SPI pins
#define SCK     PB0
#define CS      PB1
#define MOSI    PB2
#define MISO    PB3

// Initialize SPI
void init_spi();

// Send a command byte over the SPI bus
void command_spi(uint8_t);

// Read a byte from SPI
uint8_t read_spi();

#endif