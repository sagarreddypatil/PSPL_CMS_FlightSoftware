#include "adc.h"
#include "uart.h"

spi_slave_t adc_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    spi_slave_t adc = spi_slave_init(data_direction, port, slave_select);

    adc_write_register(adc, ADC_MODE_REGISTER, ADC_MODE_COMMAND);
    adc_write_register(adc, ADC_CLOCK_REGISTER, ADC_CLOCK_COMMAND);
    adc_write_register(adc, ADC_GAIN_REGISTER, ADC_GAIN_COMMAND);

    return adc;
}

uint16_t * adc_data(spi_slave_t adc) {
    static uint16_t recieve[ADC_CHANNEL_COUNT];

    spi_select(adc);
    spi_transaction(NULL, NULL, 2);
    spi_transaction(NULL, recieve, 8);
    spi_transaction(NULL, NULL, 2);
    spi_deselect(adc);

    return recieve;
}

void adc_write_register(spi_slave_t adc, uint8_t register_addr, uint16_t data) {
    uint8_t command[ADC_FRAME_SIZE];

    for(int i = 0; i < ADC_FRAME_SIZE; i++){
        command[i] = 0xBC;
        uart_printf("0x%X\n", command[i]);
    }

    spi_select(adc);
    spi_transaction(&command[0], NULL, ADC_FRAME_SIZE);
    spi_deselect(adc);
}
