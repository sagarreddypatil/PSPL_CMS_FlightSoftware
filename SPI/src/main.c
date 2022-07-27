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
#include "timer.h"

#define SWAP(num) ((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000)

int main() {
    struct spi_slave bme;

    uart_init(2000000);
    spi_init(SPI_MSB, SPI_MODE0, SPI_SPEED0);
    bme = bme280_init(&DDRB, &PORTB, BME_SS);

    uint32_t temp;
    uint64_t finish;

    while(1 == 1) {
        timer_init();
        temp = bme280_temeperature(bme);
        finish = timer_micros();
        uart_printf("Temperature: %8lX\tTook %d uS\n", (SWAP(temp)) >> 8, finish);
    }
}