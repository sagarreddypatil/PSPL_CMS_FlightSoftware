#include <stdio.h>
#include <pico/stdlib.h>

#include <w5500.h>
SPI_DEVICE(w5500, spi0, 17);

int main() {
  stdio_init_all();
  while (!stdio_usb_connected())
    ;

  for (int i = 0; i < 10; i++) {
    printf("Program: %s\n", PICO_PROGRAM_NAME);
    printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
  }

  spi_init(spi0, 5000);  // 5MHz
  gpio_set_function(16, GPIO_FUNC_SPI);
  gpio_set_function(18, GPIO_FUNC_SPI);
  gpio_set_function(19, GPIO_FUNC_SPI);

  uint actual_baud = w5500_set(w5500);
  printf("actual baud: %d\n", actual_baud);

  w5500_reset(w5500);
  uint64_t start = time_us_64();
  while (!w5500_ready(w5500))
    ;

  printf("W5500 ready, took %d us\n", (int)(time_us_64() - start));

  while (!w5500_has_link(w5500))
    ;
  printf("W5500 has link\n");
}