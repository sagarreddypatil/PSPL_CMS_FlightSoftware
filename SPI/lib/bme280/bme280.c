#include "bme280.h"
#include "uart.h"
#include "timer.h"

// Datasheet: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf

struct spi_slave bme280_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    struct spi_slave bme;
    bme = spi_slave_init(data_direction, port, slave_select);

    uint16_t command;
    command = (WRITE(0xF4) << 8);
    command += 0x37;

    uint16_t returned;

    spi_select(bme);

    spi_transaction(&command, &returned, sizeof(command)); 
    
    spi_deselect(bme);

    return bme;
}

uint32_t bme280_temeperature(struct spi_slave bme) {
    uint32_t data;
    uint32_t commands = READ(TEMPERATURE);

    spi_select(bme);
    spi_transaction(&commands, &data, 4);
    spi_deselect(bme);

    return data;
}
