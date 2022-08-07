#include "flash.h"

flash_t flash_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    flash_t flash;
    flash.slave = spi_slave_init(data_direction, port, slave_select);
    flash.page = 0;
    flash.column = 0;
    
    flash_reset(flash);

    return flash;
}

void flash_reset(flash_t flash) {
    uint8_t command = FLASH_RESET_CMD;

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_deselect(flash.slave);
}

void flash_enable(flash_t flash) {
    uint8_t command = FLASH_W_EN_CMD;

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_deselect(flash.slave);
}

void flash_disable(flash_t flash) {
    uint8_t command = FLASH_W_DIS_CMD;

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_deselect(flash.slave);
}

uint8_t flash_read_status(flash_t flash, uint8_t status_register) {
    uint8_t command = FLASH_R_STATUS_CMD;
    uint8_t value;

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_transaction(&status_register, NULL, sizeof(status_register));
    spi_transaction(NULL, &value, sizeof(value));
    spi_deselect(flash.slave);

    return value;
}

void flash_write_status(flash_t flash, uint8_t status_register, uint8_t value) {
    uint8_t command = FLASH_W_STATUS_CMD;

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_transaction(&status_register, NULL, sizeof(status_register));
    spi_transaction(&value, NULL, sizeof(value));
    spi_deselect(flash.slave);
}

void flash_block_erase(flash_t flash, uint16_t page) {
    uint8_t command = FLASH_BLOCK_ERASE_CMD;
    
    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_transaction(NULL, NULL, sizeof(uint8_t));
    spi_transaction(&page, NULL, sizeof(page));
    spi_deselect(flash.slave);
}

void flash_load_program(flash_t flash, void *data, uint8_t len, bool is_rand) {
    uint8_t command;
    if(is_rand) {
        command = FLASH_RAND_PROGRAM_CMD;
    } else {
        command = FLASH_PROGRAM_LOAD_CMD;
    }

    if((flash.column + len) > FLASH_COL_SIZE) {
        flash_program_exec(flash);
        flash.column = 0;
    }

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_transaction(&flash.column, NULL, sizeof(flash.column));
    spi_transaction(data, NULL, len);
    spi_deselect(flash.slave);

    flash.column += len;
}

void flash_program_exec(flash_t flash) {
    uint8_t command = FLASH_PROGRAM_EXEC_CMD;

    spi_select(flash.slave);
    spi_transaction(&command, NULL, sizeof(command));
    spi_transaction(NULL, NULL, sizeof(uint8_t));
    spi_transaction(&flash.page, NULL, sizeof(flash.page));
    spi_deselect(flash.slave);

    flash.page++;
}

void flash_command(flash_t flash, uint8_t *command, uint8_t *recieve, uint8_t len) {
    spi_select(flash.slave);
    spi_transaction(command, recieve, len);
    spi_deselect(flash.slave);
}