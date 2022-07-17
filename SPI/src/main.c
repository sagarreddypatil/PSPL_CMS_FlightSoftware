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

int main() {
    uart_init(2000000);

    TCCR1B = (0 << CS12) | (0 << CS11) | (1 << CS10);
    TIMSK1 = (1 << TOIE1);

    uart_printf("\nBecause of the hard work of your glorious leader, BIG CAMERON, this standard printf function can print stuff through the serial port now!\n");
    spi_init(SPI_MSB, SPI_MODE0, SPI_SPEED4);

    while(1 == 1) {
        uart_printf("0x%X\n", spi_receive());
    }
}