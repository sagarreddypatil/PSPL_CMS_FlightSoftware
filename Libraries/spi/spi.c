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
    spi_slave_t: struct that contains useful data about the specific slave that we just initialized
*/
spi_slave_t spi_slave_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    spi_slave_t slave;
    slave.port = port;
    slave.slave_select = slave_select;

    *data_direction |= (1 << slave_select);
    *port |= (1 << slave_select);

    return slave;
}

/*
Transmit a single byte over the SPI bus

Args:
    void *command: pointer to the data that we want to send to the SPI slave
    void *revieve: pointer to where we want to store the data from the SPI slave
    uint8_t len: length, in bytes, of the entire SPI transaction
*/
void spi_transaction(void *command, void *recieve, uint8_t len) {
    for(uint8_t data_idx = 0; data_idx < len; data_idx++) {
        // 18.5.3: Slap our byte into the SPI register, this bad boy can hold 8 bits!
        if(command != NULL) {
            SPDR = *(((uint8_t *)(command)) + data_idx);
        } else {
            SPDR = 0x00;
        }
        // Wait for transmission complete by looking at the SPI interrupt
        while(!SPI_INTERRUPT);
        // Fetch the byte that we just recieved over SPI
        if(recieve != NULL) {
            *(((uint8_t *)(recieve)) + data_idx) = SPDR;
        }
    }
}

/*
Toggle the slave select pin low, aka activate the slave

Args:
    spi_slave_t slave: the slave devices spi_slave_t struct
*/
void spi_select(spi_slave_t slave) {
    // Toggle the requested slave select low (active low)
    *slave.port &= ~(1 << slave.slave_select);
}

/*
Toggle the slave select pin high, aka deactivate the slave

Args:
    spi_slave_t slave: the slave devices spi_slave_t struct
*/
void spi_deselect(spi_slave_t slave) {
    // Toggle the requested slave select high
    *slave.port |= (1 << slave.slave_select);
}