#include "ethernut.h"

spi_slave_t ethernut_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    spi_slave_t eth = spi_slave_init(data_direction, port, slave_select);
    
    return eth;
}

void ethernut_frame(spi_slave_t eth, uint16_t eth_register, uint8_t block_select, uint8_t r_w, uint8_t *data, uint8_t len) {
    uint8_t control_byte = ETH_CONTROL_BYTE(block_select, r_w, ETH_VAR_LENGTH);
    eth_register = ETH_REVERSE(eth_register);

    spi_select(eth);
    
    // Initialize the ethernut SPI frame
    spi_transaction(&eth_register, NULL, sizeof(eth_register));
    spi_transaction(&control_byte, NULL, sizeof(control_byte));

    // Transmit/Recieve the number of requested bytes
    if(r_w == ETH_WRITE) {
        spi_transaction(data, NULL, len);
    } else {
        spi_transaction(NULL, data, len);
    }

    spi_deselect(eth);
}