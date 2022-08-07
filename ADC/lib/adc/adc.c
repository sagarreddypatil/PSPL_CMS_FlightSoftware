#include "adc.h"
#include "uart.h"

/*
Initialize the ADC

Args:
    volatile uint8_t *data_direction: DDRB, DDRC or DDRD, these are the "data direction" registers for each port
    volatile uint8_t *port: which port is the slave connected to? (PORTB, PORTC, PORTD)
    uint8_t slave_select: which pin is the slave connected to? (0-7)

Returns:
    spi_slave_t: struct that contains useful data about the specific slave that we just initialized
*/
spi_slave_t adc_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select) {
    spi_slave_t adc = spi_slave_init(data_direction, port, slave_select);

    adc_write_register(adc, ADC_MODE_REGISTER, ADC_MODE_COMMAND);
    adc_write_register(adc, ADC_CLOCK_REGISTER, ADC_CLOCK_COMMAND);
    adc_write_register(adc, ADC_GAIN_REGISTER, ADC_GAIN_COMMAND);

    return adc;
}

/*
Write to a register on the ADC

Args:
    spi_slave_t adc: the ADC slave device
    register_addr: the register address that we want to write to
    uint16_t data: the data that we want to write to the register
*/
void adc_write_register(spi_slave_t adc, uint8_t register_addr, uint16_t data) {
    uint16_t command[ADC_WORDS_PER_FRAME] = {0};
    command[0] = FORMAT_WREG(register_addr);
    command[1] = data;

    spi_select(adc);
    spi_transaction(command, NULL, ADC_FRAME_SIZE);
    spi_deselect(adc);
}

/*
Get all the data from the ADC

Args:
    spi_slave_t adc: the ADC slave device
    uint16_t *data: the array that we want to store the data in, has to have a length of >= 4
*/
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

/*
Useful debug function that transmits an entire SPI frame and prints it out

Args:
    spi_slave_t adc: the ADC slave device
    uint16_t *command, array of our command, length has to be >= 6
    uint16_t *revieve, array of our recieved info, length has to be >= 6
*/
void adc_print_frame(spi_slave_t adc, uint16_t *command, uint16_t *recieve) {
    spi_select(adc);
    spi_transaction(command, recieve, ADC_WORDS_PER_FRAME);
    spi_deselect(adc);

    for(uint8_t word_idx = 0; word_idx < ADC_WORDS_PER_FRAME; word_idx++) {
        uart_printf("%d\t Command: 0x%04X\tRecieve: 0x%04X\n", word_idx, command[word_idx], recieve[word_idx]);
    }
    uart_printf("\n");
}