#include "flash.h"

flash_t flash_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    flash_t flash;
    flash_t.slave = spi_slave_init(data_direction, port, slave_select);
}

void flash_reset(flash_t flash) {
    uint8_t command = FLASH_RESET_COMMAND;

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_deselect(flash.slave);
}

void flash_write_enable(flash_t flash) {
    uint8_t command = FLASH_WRITE_ENABLE;

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_deselect(flash.slave);
}

void flash_write_disable(flash_t flash) {
    uint8_t command = FLASH_WRITE_DISABLE;

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_deselect(flash.slave);
}

uint8_t flash_read_status(flash_t flash, uint8_t status_register) {
    uint8_t command = FLASH_READ_STATUS_COMMAND;
    uint8_t value;

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_transaction(&status_register, NULL, sizeof(status_register));
    spi_transaction(NULL, &value, sizeof(value));
    spi_deselect(flash.slave);
}

uint8_t flash_read_status(flash_t flash, uint8_t status_register, uint8_t value) {
    uint8_t command = FLASH_WRITE_STATUS_COMMAND;

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_transaction(&status_register, NULL, sizeof(status_register));
    spi_transaction(&value, NULL, sizeof(value));
    spi_deselect(flash.slave);
}

void flash_load_program(flash_t flash, void *data, uint8_t data_len, bool is_random) {
    uint8_t command;

    if(is_random) {
        command = FLASH_RANDOM_LOAD_PROGRAM;
    } else {
        command = FLASH_LOAD_PROGRAM;
    }

    flash_write_enable(flash);

    spi_select(flash.slave);

    spi_transaction(&command, NULL, sizeof(command));
    spi_transaction(&flash.column, NULL, sizeof(flash.column));
    spi_transaction(data, NULL, data_len);

    spi_deselect(flash.slave);

    flash_write_disable(flash);
}