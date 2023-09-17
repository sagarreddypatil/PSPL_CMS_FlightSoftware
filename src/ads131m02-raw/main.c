#include <hardware/spi.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <ads13x.h>

#define BAUD 1000000  // 1 MHz

#define ADC_SCLK 14
#define ADC_PICO 11
#define ADC_POCI 12
#define ADC_CS 15

int main() {
  stdio_init_all();
  while (!stdio_usb_connected()) tight_loop_contents();

  gpio_set_function(ADC_SCLK, GPIO_FUNC_SPI);
  gpio_set_function(ADC_PICO, GPIO_FUNC_SPI);
  gpio_set_function(ADC_POCI, GPIO_FUNC_SPI);
  spi_init(spi1, BAUD);

  gpio_init(ADC_CS);
  gpio_put(ADC_CS, 1);
  gpio_set_dir(ADC_CS, GPIO_OUT);

  spi_set_format(spi1, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);

  while (true) {
    uint8_t src[4 * 3] = {0, 0, 0};
    uint8_t dst[4 * 3] = {0};

    gpio_put(ADC_CS, 0);
    spi_write_read_blocking(spi1, src, dst, 4 * 3);
    gpio_put(ADC_CS, 1);

    for (int i = 0; i < 4 * 3; i++) {
      printf("%02x ", dst[i]);
    }
    printf("\n");
    sleep_ms(100);
  }
}
