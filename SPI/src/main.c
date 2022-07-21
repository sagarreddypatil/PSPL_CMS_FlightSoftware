/*
Built by Mr. Dr. Lord Master God Emperor Supreme Chancelor Overlord etc etc etc Big Cameron himself

Here Big Cameron from the planet Earth first started coding CMS, July 10th, 2022

We came in hate for Blue Origin

Starting this project was kinda like this: https://youtu.be/vFUx_KC1bHQ?t=77

atmega328p datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

https://youtu.be/4gHivrcJ-YY?t=45
*/
#include "spi.h"
#include "uart.h"
#include "bme280.h"

int main() {
    uart_init(2000000);
    spi_init(SPI_MSB, SPI_MODE0, SPI_SPEED0);
    bme280_init(&DDRB, &PORTB, BME_SS);

    uint32_t temp;

    while(1 == 1) {
        temp = bme280_temeperature();
        uart_printf("Temperature: %lX\n", temp);
    }
}