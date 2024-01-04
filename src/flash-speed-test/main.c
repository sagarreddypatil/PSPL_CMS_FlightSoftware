#include <stdio.h>
#include <pico/stdlib.h>
#include <w25n01.h>
#include <ads13x.h>
#include <max31856.h>
#include <psp.h>
#include <pico/time.h>

SPI_DEVICE(tc_0, spi0, 1);
SPI_DEVICE(tc_1, spi0, 0);
SPI_DEVICE(adc_0, spi0, 6);
SPI_DEVICE(flash, spi0, 20);

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

  max31856_set(tc_1);
  ads13x_set(adc_0);
  w25n01_set(flash);

  printf("Speed Test Program Start\n");

  uint8_t write_data[2048] = {0};

  absolute_time_t start = get_absolute_time();
  for (uint16_t i = 0; i < 65536; i++) {
    // printf("Writing page %d... ", i);
    // absolute_time_t write_start = get_absolute_time();
    w25n01_write_buffer(flash, 0, write_data, 2048, true);

    w25n01_write_enable(flash);
    w25n01_write_page(flash, i);

    while (w25n01_get_busy(flash))
      ;
    // int64_t time_taken = absolute_time_diff_us(write_start,
    // get_absolute_time()); printf("took %lld us \n", time_taken);
  }

  int64_t time_taken = absolute_time_diff_us(start, get_absolute_time());
  for (int i = 0; i < 10; i++) {
    printf("Done Writing!\n");
    printf("Total time to write 65536 pages: %lld\n", time_taken);
  }

  start = get_absolute_time();

  uint64_t read_data[256] = {0};
  for (uint16_t i = 0; i < 65536; i++) {
    w25n01_read_page(flash, i);
    while (w25n01_get_busy(flash))
      ;

    w25n01_read_buffer(flash, 0, read_data, 2048);

    for (int j = 0; j < 256; j++) {
      if (read_data[j] != 0) {
        printf("Writing Failed!!!\n");
        goto fail;
      }
    }

    while (w25n01_get_busy(flash))
      ;
  }

  time_taken = absolute_time_diff_us(start, get_absolute_time());

  for (int i = 0; i < 10; i++) {
    printf("Done Reading!\n");
    printf("Total time to read 65536 pages: %lld\n", time_taken);
  }

fail:
  while (true)
    ;
}
