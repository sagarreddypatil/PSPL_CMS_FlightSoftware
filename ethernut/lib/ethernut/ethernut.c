#include "ethernut.h"

spi_slave_t ethernut_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    spi_slave_t eth = spi_slave_init(data_direction, port, slave_select);
    
    return eth;
}

void ethernut_write(spi_slave_t eth, uint16_t eth_register, uint8_t command_byte, uint8_t data) {
    uint8_t command[4];

    command[0] = ((eth_register & 0xFF00) >> 8);
    command[1] = (eth_register & 0xFF);
    command[2] = command_byte;
    command[3] = data;

    spi_select(eth);
    spi_transaction(command, NULL, 4);
    spi_deselect(eth);
}