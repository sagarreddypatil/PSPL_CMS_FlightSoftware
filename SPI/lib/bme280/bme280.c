#include "bme280.h"

// Datasheet: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf

void bme280_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    slave_init(data_direction, port, slave_select);

    spi_select(&PORTB, SS);
    spi_transmit(WRITE(0xF4));
    spi_transmit(0x37);
    spi_deselect(&PORTB, SS);
}

uint8_t bme280_id() {
    uint8_t id;

    spi_select(&PORTB, SS);

    spi_transmit(READ(ID));

    id = spi_receive();

    spi_deselect(&PORTB, SS);

    return id;
}

uint16_t bme280_humidity() {
    uint16_t data = 0;

    spi_select(&PORTB, SS);

    spi_transmit(READ(HUMIDITY));

    data = (data << 8) + spi_receive();
    data = (data << 8) + spi_receive();

    spi_deselect(&PORTB, SS);

    return data;
}

uint32_t bme280_pressure() {
    uint32_t data = 0;

    spi_select(&PORTB, SS);

    spi_transmit(READ(PRESSURE));

    data = (data << 8) + spi_receive();
    data = (data << 8) + spi_receive();
    data = (data << 8) + spi_receive();

    spi_deselect(&PORTB, SS);

    return data;
}

uint32_t bme280_temeperature() {
    uint32_t data = 0;

    spi_select(&PORTB, SS);

    spi_transmit(READ(TEMPERATURE));

    data = (data << 8) + spi_receive();
    data = (data << 8) + spi_receive();
    data = (data << 8) + spi_receive();

    spi_deselect(&PORTB, SS);

    return data;
}
