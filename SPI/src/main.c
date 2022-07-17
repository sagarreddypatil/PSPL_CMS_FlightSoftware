/*
Built by Mr. Dr. Lord Master God Emperor Supreme Chancelor Overlord etc etc etc Big Cameron himself

Here Big Cameron from the planet Earth first started coding CMS, July 10th, 2022

We came in hate for Blue Origin

Starting this project was kinda like this: https://youtu.be/vFUx_KC1bHQ?t=77

atmega328p datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

https://youtu.be/4gHivrcJ-YY?t=45
*/
#include <stdio.h.>
#include "spi.h"
#include "uart.h"
#include "bme280.h"

int main() {
    uart_init();
    // Set stdio to the serial connection
    FILE uart_io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
    stdin = stdout = &uart_io;

    printf("\nBecause of the hard work of your glorious leader, BIG CAMERON, this standard printf function can print stuff through the serial port now!\n");
    printf("Serial speed is dependent on baud rate so faster == better but faster also == less stable.\n");
    printf("Current speed is %ldbps, techincally you can get 2Mbps but the current version of that is kinda buggy.\n", BAUD);

    spi_init(SPI_MSB, SPI_MODE0, SPI_SPEED4);
    bme280_init(PORTB, BME_SS);

    while(1 == 1) {
        getchar();
        printf("SPI Data Register: 0x%lX\n", bme280_humidity());
    }
}