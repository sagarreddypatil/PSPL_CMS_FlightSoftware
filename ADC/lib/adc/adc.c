#include "adc.h"

spi_slave_t adc_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    spi_slave_t adc = spi_slave_init(data_direction, port, slave_select);

    return adc;
}

uint32_t adc_data(spi_slave_t adc, uint8_t channel) {
    uint32_t data;
    uint32_t command = channel;

    spi_select(adc);
    spi_transaction(&command, &data, sizeof(command));
    spi_deselect(adc);

    return data;
}