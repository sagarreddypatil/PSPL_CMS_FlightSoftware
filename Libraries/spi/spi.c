#include "spi.h"
#include "uart.h"

/*
Initialize the SPI Bus

Args:
    uint8_t data_order: is data going to be MSB (0) or LSB (1)?
    uint8_t spi_mode: what SPI mode does the slave device need? (0, 1, 2, 3)
    uint8_t sck_speed: what clock speed do we want to use? described on and around page 141 of the datasheet
*/
void spi_init(uint8_t data_order, uint8_t spi_mode, uint8_t sck_speed) {
    // 13.4.3: SPI output pins on port B: MOSI, SCK and Master SS
    DDRB = ((1 << MOSI) | (1 << SCK) | (1 << SS)); 

    // 18.5.1: Configure the SPI bus to be whatever the user requested
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

/*
Initialize the slave select pin for a new slave device

Args:
    volatile uint8_t *data_direction: DDRB, DDRC or DDRD, these are the "data direction" registers for each port
    volatile uint8_t *port: which port is the slave connected to? (PORTB, PORTC, PORTD)
    uint8_t slave_select: which pin is the slave connected to? (0-7)
*/
void spi_slave_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    *data_direction |= (1 << slave_select);
    *port |= (1 << slave_select);
}

/*
Transmit a single byte over the SPI bus

Args:
    uint8_t data_byte: the byte that we want to send
*/
void spi_transmit(uint8_t data_byte) {
    // 18.5.3: Slap our byte into the SPI register, this bad boy can hold 8 bits!
    SPI_DATA_REGISTER = data_byte;
    // Wait for transmission complete by looking at the SPI interrupt
    while(!SPI_INTERRUPT);
}


/*
Receive a single byte over the SPI bus

Returns:
    uint8_t: the contents of the data register, aka the byte we want
*/
uint8_t spi_receive() {
    // 18.5.3: Transmit BC for Big Cameron, doesn't really matter what's here
    SPI_DATA_REGISTER = 0xBC;
    // Wait for the transmission to be complete by looking at the SPI interrupt
    while(!SPI_INTERRUPT);
    // Return the byte that we just recieved over SPI
    return SPI_DATA_REGISTER;
}

/*
Toggle the slave select pin low, aka activate the slave

Args:
    volatile uint8_t *port: which port is the slave connected to? (PORTB, PORTC, PORTD)
    uint8_t slave_select: which pin is the slave connected to? (0-7)
*/
void spi_select(volatile uint8_t *port, uint8_t slave_select) {
    // Toggle the requested slave select low (active low)
    *port &= ~(1 << slave_select);
}

/*
Toggle the slave select pin high, aka deactivate the slave

Args:
    volatile uint8_t *port: which port is the slave connected to? (PORTB, PORTC, PORTD)
    uint8_t slave_select: which pin is the slave connected to? (0-7)
*/
void spi_deselect(volatile uint8_t *port, uint8_t slave_select) {
    // Toggle the requested slave select high
    *port |= (1 << slave_select);
}