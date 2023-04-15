
#include <pico/stdlib.h>
#include <stdio.h>
#include <w5500.h>

// SPI_DEVICE(w5500, spi0, 0);
int main() {
  stdio_init_all();

  // spi_init(spi0, 1000);
  // gpio_set_function(2, GPIO_FUNC_SPI);
  // gpio_set_function(3, GPIO_FUNC_SPI);
  // gpio_set_function(4, GPIO_FUNC_SPI);

  // for (int i = 0; i < 10; i++) {
  //   printf("debug point 0 %d\n", i);
  //   sleep_ms(1000);
  // }

  // uint actual_baud = w5500_set(w5500);

  // ip_t gateway     = {192, 168, 2, 50};
  // ip_t subnet_mask = {0xff, 0xff, 0xff, 0x00};
  // ip_t src_ip      = {192, 168, 2, 101};
  // mac_t src_mac    = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};
}