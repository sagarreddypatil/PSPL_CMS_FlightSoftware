#include "bme280.h"
#include "uart.h"
#include "timer.h"

// Datasheet: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf

struct spi_slave bme280_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    struct spi_slave bme;
    bme = spi_slave_init(data_direction, port, slave_select);

    spi_select(bme);
    
    spi_transaction(WRITE(0xF4));
    spi_transaction(0x37);
    
    spi_deselect(bme);

    return bme;
}

uint8_t bme280_id(struct spi_slave bme) {
    uint8_t id;

    spi_select(bme);

    spi_transaction(READ(ID));

    id = spi_transaction(0x00);

    spi_deselect(bme);

    return id;
}

uint16_t bme280_humidity(struct spi_slave bme) {
    uint16_t data = 0;

    spi_select(bme);

    spi_transaction(READ(HUMIDITY));

    data = spi_transaction(0x00);
    data = (data << 8) + spi_transaction(0x00);

    spi_deselect(bme);

    return data;
}

uint32_t bme280_pressure(struct spi_slave bme) {
    uint32_t data = 0;

    spi_select(bme);

    spi_transaction(READ(PRESSURE));

    data = spi_transaction(0x00);
    data = (data << 8) + spi_transaction(0x00);
    data = (data << 8) + spi_transaction(0x00);

    spi_deselect(bme);

    return data;
}

uint32_t bme280_temeperature(struct spi_slave bme) {
    uint32_t data;

    spi_select(bme);

    spi_transaction(READ(TEMPERATURE));

    data = spi_transaction(0x00);
    data = (data << 8) + spi_transaction(0x00);
    data = (data << 8) + spi_transaction(0x00);

    spi_deselect(bme);

    return data;
}
