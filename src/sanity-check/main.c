#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/time.h>

#include <w5500.h>
SPI_DEVICE(w5500, spi1, 25);

int main() {
  stdio_init_all();
  while (!stdio_usb_connected())
    ;

  for (int i = 0; i < 10; i++) {
    printf("Program: %s\n", PICO_PROGRAM_NAME);
    printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
  }

  spi_init_bus(spi1, 26, 27, 28);

  uint actual_baud = w5500_set(w5500);
  printf("actual baud: %d\n", actual_baud);

  w5500_reset(w5500);
  uint64_t start = time_us_64();
  while (!w5500_ready(w5500))
    ;

  printf("W5500 ready, awaiting link, took %d us\n",
         (int)(time_us_64() - start));

  while (!w5500_has_link(w5500)) {
    uint8_t phycfgr = w5500_read8(w5500, W5500_COMMON, W5500_PHYCFGR);
    printf("0x%x\n", phycfgr);
    sleep_ms(250);
  };

  printf("W5500 has link\n");

  while (true) {
    uint8_t phycfgr = w5500_read8(w5500, W5500_COMMON, W5500_PHYCFGR);
    printf("0x%x\n", phycfgr);
    sleep_ms(250);
  };
}
