#include "bme280.h"

uint16_t bme280_humidity() {
    uint16_t data;

    spi_select(BME_SS);

    spi_transmit(HUMIDITY);

    data = (spi_receive() << 8);
    data |= spi_receive();

    spi_deselect(BME_SS);
}

uint32_t bme280_pressure() {
    uint32_t data;

    spi_select(BME_SS);

    spi_transmit(PRESSURE);

    data = (spi_receive() << 16);
    data |= (spi_receive() << 8);
    data |= spi_receive();

    spi_deselect(BME_SS);
}

uint32_t bme280_temeperature() {
    uint32_t data;

    spi_select(BME_SS);

    spi_transmit(TEMPERATURE);

    data = (spi_receive() << 16);
    data |= (spi_receive() << 8);
    data |= spi_receive();

    spi_deselect(BME_SS);
}