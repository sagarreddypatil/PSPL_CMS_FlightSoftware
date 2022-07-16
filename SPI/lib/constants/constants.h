#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <avr/io.h>

/*
    SPI
*/

// SPI Data Direction Register
#define SPI_DDR DDRB

// SPI pins
#define MOSI PINB3
#define MISO PINB4
#define SCK PINB5

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

/*
    UART
*/

// Set Baud Rate
#ifndef BAUD
#define BAUD 115200
#endif
#include <util/setbaud.h>

#endif