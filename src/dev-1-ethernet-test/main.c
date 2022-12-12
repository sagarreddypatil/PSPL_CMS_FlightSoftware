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
  uint8_t data[6];
  ip_addr_t ip = {192,168,0,1};
  ip_addr_t gateway = {1,1,1,1};
  ip_addr_t subnet_mask = {255,255,0,0};
  mac_t mac  = {1, 2, 4, 8, 16, 32};
  printf("debug point 1, actual baud %d\n", actual_baud);
  w5500_configIP(w5500, ip, gateway, subnet_mask, mac);
  while (true) {
    w5500_rw(w5500, w5500_shar0, cmn, data, sizeof(data), false);
    printf("%x %x %x %x %x %x\n", data[0], data[1], data[2], data[3], data[4], data[5]);
    sleep_ms(500);

    
    // w5500_rw(w5500, w5500_mr, cmn, &data, 1, false);
    // printf("0x%x\n", data);

  }
}