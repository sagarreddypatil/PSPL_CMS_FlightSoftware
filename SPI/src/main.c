/*
Built by Mr. Dr. Lord Master God Emperor Supreme Chancelor Overlord etc etc etc Big Cameron himself

Here Big Cameron from the planet Earth first started coding CMS, July 10th, 2022

We came in hate for Blue Origin

Starting this project was kinda like this: https://youtu.be/vFUx_KC1bHQ?t=77

atmega328p datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
*/
#include <stdio.h>
#include "spi.h"
#include "uart.h"

int main() {
    uart_init();

    stdout = &uart_output;
    stdin  = &uart_input;

    printf("\nThis standard printf function can print stuff through the serial port now!\n");
    printf("Serial speed is dependent on baud rate so faster == better but faster also == less stable.\n");
}