#include <stdio.h>
#include <pico/stdlib.h>

#include <w25n01.h>

SPI_DEVICE(flash0, spi0, 15);
SPI_DEVICE(flash1, spi0, 14);

int main() {
    stdio_init_all();
    while (!stdio_usb_connected())
        ;

    for (int i = 0; i < 10; i++) {
        printf("Program: %s\n", PICO_PROGRAM_NAME);
        printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
    }

    spi_init_bus(spi0, 2, 3, 4);
    uint actual_baud = w25n01_set(flash0);

    printf("actual baud: %d\n", actual_baud);

    while (true) {
        uint8_t status =
            w25n01_read_status_register(flash0, w25n01_status_register);
        printf("status: 0x%x\n", status);
        sleep_ms(1000);
    }
}