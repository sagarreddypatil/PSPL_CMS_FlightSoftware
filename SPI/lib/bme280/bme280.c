#include "bme280.h"
#include "uart.h"
#include "timer.h"

// Datasheet: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf

spi_slave bme280_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    spi_slave bme = spi_slave_init(data_direction, port, slave_select);
    
    uint16_t init_command = 0xA374;

    spi_select(bme);
    spi_transaction(&init_command, NULL, sizeof(init_command));
    spi_deselect(bme);
    
    return bme;
}

uint32_t bme280_temeperature(spi_slave bme) {
    uint32_t temp;
    uint32_t commands = 0xFA;

    spi_select(bme);
    spi_transaction(&commands, &temp, sizeof(commands));
    spi_deselect(bme);

    temp = ((SWAP(temp)) & 0x00FFFFFF) >> 4;

    return temp;
}
