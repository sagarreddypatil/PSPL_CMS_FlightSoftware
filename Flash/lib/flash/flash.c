/*
Flash Datasheet: https://www.winbond.com/resource-files/w25n01gv%20revl%20050918%20unsecured.pdf
*/

#include "flash.h"

uint16_t page;
uint16_t column;

void flash_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    struct spi_device flash;

    flash.port = port;
    flash.slave_select = slave_select;

    spi_slave_init(data_direction, port, slave_select);

    spi_select(port, slave_select);
    spi_transmit(RESET_COMMAND);
    spi_select(port, slave_select);
}

uint32_t flash_jedec(struct spi_device flash) {
    uint32_t jedec_id;

    spi_select(flash.port, flash.slave_select);

    spi_transmit(JEDEC_COMMAND);
    spi_receive();

    jedec_id = spi_receive();
    jedec_id = (jedec_id << 8) + spi_receive();
    jedec_id = (jedec_id << 8) + spi_receive();

    spi_deselect(flash.port, flash.slave_select);

    return jedec_id;
}

uint8_t flash_read_status(struct spi_device flash, uint8_t register) {
    uint8_t status;

    spi_select(flash.port, flash.slave_select);

    spi_transmit(STATUS_COMMAND);
    spi_transmit(register);

    status = spi_receive();

    spi_deselect(flash.port, flash.slave_select);

    return status;
}

void flash_write_status(struct spi_device flash, uint8_t register, uint8_t value) {
    spi_select(flash.port, flash.slave_select);

    spi_transmit(STATUS_COMMAND);
    spi_transmit(register);
    spi_transmit(value);

    spi_deselect(flash.port, flash.slave_select);
}

void flash_read_bbm_table(struct spi_device flash) {
    spi_select(flash.port, flash.slave_select);

    spi_transmit(READ_BBM);
    spi_receive();

    spi_deselect(flash)
}