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
#include "timer.h"

int main() {
    uart_init();

    TCCR1B = (0 << CS12) | (0 << CS11) | (1 << CS10);
    TIMSK1 = (1 << TOIE1);

    // Set stdio to the serial connection
    FILE uart_io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
    stdin = stdout = &uart_io;

    printf("\nBecause of the hard work of your glorious leader, BIG CAMERON, this standard printf function can print stuff through the serial port now!\n");
    printf("Serial speed is dependent on baud rate so faster == better but faster also == less stable.\n");
    printf("Current speed is %ldbps, techincally you can get 2Mbps but the current version of that is kinda buggy.\n", BAUD);

    spi_init(SPI_MSB, SPI_MODE0, SPI_SPEED4);
    bme280_init(PORTB, BME_SS);

    while(1 == 1) {
        uint16_t start = ((TCNT1H << 8) + TCNT1L);
        uint16_t end = ((TCNT1H << 8) + TCNT1L);
        printf("%d, %d\n", start, end);
    }

    /*
    while(1 == 1) {
        printf("128 bit (16 byte) data type length: %d bits, %d bytes\n", sizeof(data) * 8, sizeof(data));
        getchar();
        printf("SPI Data Register: 0x%lX\n", bme280_humidity());
    }
    */
}