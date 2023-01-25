#include <hardware/spi.h>
#include <pico/stdlib.h>
#include <stdio.h>

#define DEFAULT_BAUD 1000000  // 1 MHz

int main() {
  stdio_init_all();

  spi_init(spi0, DEFAULT_BAUD);
  gpio_set_function(2, GPIO_FUNC_SPI);  // SCK
  gpio_set_function(3, GPIO_FUNC_SPI);  // TX
  gpio_set_function(4, GPIO_FUNC_SPI);  // RX

  spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);

  while (true) {
    uint8_t src[4 * 3] = {0};
    uint8_t dst[4 * 3] = {0};
    spi_write_read_blocking(spi0, src, dst, 4 * 3);

    for (int i = 0; i < 4 * 3; i++) {
      printf("%02x ", dst[i]);
    }
    printf("\n");
    sleep_ms(100);
  }
}
