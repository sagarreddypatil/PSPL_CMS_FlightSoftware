/*
Flash Datasheet: https://www.winbond.com/resource-files/w25n01gv%20revl%20050918%20unsecured.pdf
*/

void flash_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    spi_slave_init(data_direction, port, slave_select);
}

void flash_write(volatile uint8_t *port, uint8_t slave_select, uint8_t data) {
    spi_select(port, slave_select);

    uint16_t page;
    uint16_t column;



    spi_deselect(port, slave_select);
}