#include "bme280.h"

// Datasheet: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf

void bme280_init(uint8_t port, uint8_t slave_select) {
    port |= (1 << slave_select);
}

uint16_t bme280_humidity() {
    uint16_t data = 0;

    spi_select(PORTB, BME_SS);

    spi_transmit(READ(HUMIDITY));

    data = (data << 8) + spi_receive();
    data = (data << 8) + spi_receive();

    spi_deselect(PORTB, BME_SS);

    return data;
}

uint32_t bme280_pressure() {
    uint32_t data = 0;

    spi_select(PORTB, BME_SS);

    spi_transmit(READ(PRESSURE));

    data = (data << 8) + spi_receive();
    data = (data << 8) + spi_receive();
    data = (data << 8) + spi_receive();

    spi_deselect(PORTB, BME_SS);

    return data;
}

uint32_t bme280_temeperature() {
    uint32_t data = 0;

    spi_select(PORTB, BME_SS);

    spi_transmit(READ(TEMPERATURE));

    data = (data << 8) + spi_receive();
    data = (data << 8) + spi_receive();
    data = (data << 8) + spi_receive();

    spi_deselect(PORTB, BME_SS);

    return data;
}