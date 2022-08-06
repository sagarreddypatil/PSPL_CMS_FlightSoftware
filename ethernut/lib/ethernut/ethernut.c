#include "ethernut.h"

spi_slave_t ethernut_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    spi_slave_t eth = spi_slave_init(data_direction, port, slave_select);
    
    return eth;
}