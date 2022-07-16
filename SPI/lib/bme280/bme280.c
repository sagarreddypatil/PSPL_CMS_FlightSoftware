#include "bme280.h"

// datasheet https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf

uint16_t bme280_humidity() {
    uint16_t data;

    spi_select(BME_SS);

    spi_transmit(READ + HUMIDITY);

    data = spi_receive(2);

    spi_deselect(BME_SS);

    return data;
}

uint32_t bme280_pressure() {
    uint32_t data;

    spi_select(BME_SS);

    spi_transmit(READ + PRESSURE);

    data = spi_receive(3);

    spi_deselect(BME_SS);

    return data;
}

uint32_t bme280_temeperature() {
    uint32_t data;

    spi_select(BME_SS);

    spi_transmit(READ + TEMPERATURE);

    data = spi_receive(3);

    spi_deselect(BME_SS);

    return data;
}