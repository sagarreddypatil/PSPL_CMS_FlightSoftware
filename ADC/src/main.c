// Big Cameron Was Here
#include "adc.h"
#include "uart.h"
#include "timer.h"

int main() {
    uart_init(2000000);
    spi_init(SPI_MSB, SPI_MODE0, SPI_SPEED_PLAID);
    spi_slave_t adc = adc_init(&DDRB, &PORTB, PINB0);

    uint16_t data[ADC_CHANNEL_COUNT];
    uint64_t finish;

    while(true) {
        timer_init();
        adc_data(adc, data);
        finish = timer_micros();
        uart_printf("{%04X, %04X, %04X, %04X} in %lduS\n", data[0], data[1], data[2], data[3], finish); 
    }
}