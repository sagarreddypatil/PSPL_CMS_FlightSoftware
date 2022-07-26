/*
Flash Datasheet: https://www.winbond.com/resource-files/w25n01gv%20revl%20050918%20unsecured.pdf
*/

#include "flash.h"

void flash_reset(flash_t flash) {
    spi_select(flash.slave);
    spi_transaction(RESET_COMMAND);
    spi_deselect(flash.slave);
}

void flash_write_enable(flash_t flash) {
    spi_select(flash.slave);
    spi_transaction(WRITE_ENABLE);
    spi_deselect(flash.slave);
}

void flash_write_disable(flash_t flash) {
    spi_select(flash.slave);
    spi_transaction(WRITE_DISABLE);
    spi_deselect(flash.slave);
}

void flash_load_program(flash_t flash, uint8_t *data, uint32_t data_len, bool is_random) {
    spi_select(flash.slave);

    if(is_random) {
        spi_transaction(RANDOM_LOAD_PROGRAM);
    }
    else {
        spi_transaction(LOAD_PROGRAM);
    }

    spi_transaction(flash.column >> 8);
    spi_transaction(flash.column);

    for(uint32_t data_idx = 0; data_idx < data_len; data_idx++) {
        spi_transaction(data[data_idx]);
    }

    spi_deselect(flash.slave);
}