#include <pico/stdlib.h>
#include <stdio.h>
#include <w5500.h>

SPI_DEVICE(w5500, spi0, 0);
int main() {
  stdio_init_all();

  spi_init(spi0, 1000 * 1000);
  gpio_set_function(2, GPIO_FUNC_SPI);
  gpio_set_function(3, GPIO_FUNC_SPI);
  gpio_set_function(4, GPIO_FUNC_SPI);

  for (int i = 0; i < 10; i++) {
    printf("debug point 0 %d\n", i);
    sleep_ms(1000);
  }

  uint actual_baud = w5500_set(w5500);
  ip_t gateway = {0x02, 0x06, 0x07, 0x08};
  ip_t subnet = {0xff, 0xff, 0x00, 0x00};
  ip_t src = {192, 168, 2, 1};
  mac_t mac = {0x08, 0xDC, 0x0A, 0x01, 0x10, 0x18};

  printf("debug point 1, actual baud %d\n", actual_baud);
  w5500_init(w5500, gateway, subnet, src, mac);

  while (true) {
    w5500_status(w5500);
    sleep_ms(500);
    
    // w5500_rw(w5500, w5500_mr, cmn, &data, 1, false);
    // printf("0x%x\n", data);

  }
}