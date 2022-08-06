/*
Flash Datasheet: https://www.winbond.com/resource-files/w25n01gv%20revl%20050918%20unsecured.pdf
*/

#include "flash.h"

void flash_reset(flash_t flash) {
    uint8_t command = RESET_COMMAND;

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_deselect(flash.slave);
}

void flash_write_enable(flash_t flash) {
    uint8_t command = WRITE_ENABLE;

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_deselect(flash.slave);
}

void flash_write_disable(flash_t flash) {
    uint8_t command = WRITE_DISABLE;

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_deselect(flash.slave);
}

void flash_load_program(flash_t flash, void *data, uint8_t data_len, bool is_random) {
    uint8_t command;

    if(is_random) {
        command = RANDOM_LOAD_PROGRAM;
    }
    else {
        command = LOAD_PROGRAM;
    }

    spi_select(flash.slave);

    spi_transaction(&command, NULL, sizeof(command));
    spi_transaction(&flash.column, NULL, sizeof(flash.column));
    spi_transaction(data, NULL, data_len);

    spi_deselect(flash.slave);
}