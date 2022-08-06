#include "adc.h"
#include "uart.h"

spi_slave_t adc_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    spi_slave_t adc = spi_slave_init(data_direction, port, slave_select);

    adc_write_register(adc, ADC_MODE_REGISTER, ADC_MODE_COMMAND);
    adc_write_register(adc, ADC_CLOCK_REGISTER, ADC_CLOCK_COMMAND);
    adc_write_register(adc, ADC_GAIN_REGISTER, ADC_GAIN_COMMAND);

    return adc;
}

void adc_write_register(spi_slave_t adc, uint8_t register_addr, uint16_t data) {
    uint16_t command[ADC_WORDS_PER_FRAME] = {0};
    command[0] = FORMAT_WREG(register_addr);
    command[1] = data;

    spi_select(adc);
    spi_transaction(command, NULL, ADC_FRAME_SIZE);
    spi_deselect(adc);
}

void adc_data(spi_slave_t adc, uint16_t *data) {
    spi_select(adc);

    // Skip the first word of the frame
    spi_transaction(NULL, NULL, ADC_WORD_SIZE);
    // Read the data, get 1 word for each channel
    spi_transaction(NULL, data, ADC_CHANNEL_COUNT * ADC_WORD_SIZE);
    // Skip the last word of the frame
    spi_transaction(NULL, NULL, ADC_WORD_SIZE);

    spi_deselect(adc);
}

void adc_print_frame(uint16_t *command, uint16_t *recieve) {
    uart_printf("\n");
    for(uint8_t word_idx = 0; word_idx < ADC_WORDS_PER_FRAME; word_idx++) {
        uart_printf("%d\t Command: 0x%04X\tRecieve: 0x%04X\n", word_idx, command[word_idx], recieve[word_idx]);
    }
}