#include "spi.h"

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

        (((sck_speed & 2) >> 1) << SPR1) |
        ((sck_speed & 1) << SPR0)
    );

    // 18.5.2: Configure the SPI 2x speed if we want that
    SPSR = (((sck_speed & 4) >> 2) << SPI2X);
}

/*
Initialize the slave select pin for a new slave device
Args:
    volatile uint8_t *data_direction: DDRB, DDRC or DDRD, these are the "data direction" registers for each port
    volatile uint8_t *port: which port is the slave connected to? (PORTB, PORTC, PORTD)
    uint8_t slave_select: which pin is the slave connected to? (0-7)
Returns:
    struct spi_slave: struct that contains useful data about the specific slave that we just initialized
*/
struct spi_slave spi_slave_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    struct spi_slave slave;
    slave.port = port;
    slave.slave_select = slave_select;

    *data_direction |= (1 << slave_select);
    *port |= (1 << slave_select);

    return slave;
}

/*
Transmit a single byte over the SPI bus
Args:
    uint8_t data_byte: the byte that we want to sends
Returns:
    uint8_t: the byte we read over the SPI bus
*/
uint8_t spi_transaction(uint8_t data_byte) {
    // 18.5.3: Slap our byte into the SPI register, this bad boy can hold 8 bits!
    SPDR = data_byte;
    // Wait for transmission complete by looking at the SPI interrupt
    while(!SPI_INTERRUPT);
    // Return the byte that we just recieved over SPI
    return SPDR;
}

/*
Toggle the slave select pin low, aka activate the slave
Args:
    struct spi_slave slave: the slave devices spi_slave struct
*/
void spi_select(struct spi_slave slave) {
    // Toggle the requested slave select low (active low)
    *slave.port &= ~(1 << slave.slave_select);
}

/*
Toggle the slave select pin high, aka deactivate the slave
Args:
    struct spi_slave slave: the slave devices spi_slave struct
*/
void spi_deselect(struct spi_slave slave) {
    // Toggle the requested slave select high
    *slave.port |= (1 << slave.slave_select);
}