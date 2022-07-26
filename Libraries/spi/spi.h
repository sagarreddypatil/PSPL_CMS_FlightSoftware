#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

// SPI pins
#define SS PINB2
#define MOSI PINB3
#define MISO PINB4
#define SCK PINB5

// SPI MSB or LSB
#define SPI_MSB 0
#define SPI_LSB 1

// Table 18-2, 18-3 and 18-4: SPI Modes
#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3

// Table 18-5: SPI Clock Speeds from fastest to slowest
#define SPI_SPEED0 4 // f_osc/2
#define SPI_SPEED1 0 // f_osc/4
#define SPI_SPEED2 5 // f_osc/8
#define SPI_SPEED3 1 // f_osc/16
#define SPI_SPEED4 6 // f_osc/32
#define SPI_SPEED5 2 // f_osc/64
#define SPI_SPEED6 3 // f_osc/128

#define SPI_INTERRUPT (SPSR & (1 << SPIF))

/*
SPI Slave Struct
    volatile uint8_t *port: what port this device uses (PORTB, PORTC, PORTD)
    uint8_t slave_select: what slave select pin this device uses (0-7)
*/
struct spi_slave {
  volatile uint8_t *port;
  uint8_t slave_select;
};

void spi_init(uint8_t, uint8_t, uint8_t);
struct spi_slave spi_slave_init(volatile uint8_t*, volatile uint8_t*, uint8_t);
uint8_t spi_transaction(uint8_t);
void spi_select(struct spi_slave);
void spi_deselect(struct spi_slave);

#endif