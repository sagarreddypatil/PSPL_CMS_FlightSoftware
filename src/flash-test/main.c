#include <stdio.h>
#include <pico/stdlib.h>
#include <w25n01.h>
#include <ads13x.h>
#include <max31856.h>
#include <psp.h>

SPI_DEVICE(tc_0, spi0, 1);
SPI_DEVICE(tc_1, spi0, 0);
SPI_DEVICE(adc_0, spi0, 6);
SPI_DEVICE(flash, spi0, 20);
// SPI_DEVICE(w5500, spi1, 25);

int main() {
    stdio_init_all();
    while (!stdio_usb_connected())
        ;

    for (int i = 0; i < 10; i++) {
        printf("\n");
    }

    printf(psp_logo);
    printf("Program: %s\n", PICO_PROGRAM_NAME);
    printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);

    spi_init_bus(spi0, 2, 3, 4);

    max31856_set(tc_0);
    max31856_set(tc_1);
    ads13x_set(adc_0);
    w25n01_set(flash);

    // spi_init_bus(spi1, 26, 27, 28);
    // w5500_set(w5500);

    printf("Flash Test Program Start\n");

    int bad_blocks = 0;

    for (int i = 0; i < 1024; i++) {  // there are 1024 blocks
        uint16_t page_addr = i * 64;

        w25n01_read_page(flash, page_addr);
        while (w25n01_get_busy(flash)) {
            // wait till not busy
        }

        uint16_t bad_block_marker = 0;
        w25n01_read_buffer(flash, 0x0, &bad_block_marker,
                           2);  // 0x800 is location of spare area

        printf("block %d, page %u, marker %x\n", i, page_addr,
               bad_block_marker);
        if (bad_block_marker != 0xffff) {
            bad_blocks += 1;
        }
    }

    printf("\nSummary: %d bad blocks\n", bad_blocks);

    while (true)
        ;
}
