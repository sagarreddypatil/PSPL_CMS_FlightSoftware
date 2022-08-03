// Big Cameron Was Here
#include "adc.h"
#include "uart.h"
#include "timer.h"

int main() {
    uart_init(2000000);
    timer_init();

    spi_slave_t adc = adc_init(&DDRB, &PORTB, PINB0);

    uart_printf("Done!\n");
}