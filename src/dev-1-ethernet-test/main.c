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
  ip_t s0_dst = {1, 2, 3, 4};
  uint16_t s0_src_port = 5000;
  uint16_t s0_dst_port = 5000;

  printf("debug point 1, actual baud %d\n", actual_baud);
  w5500_init(w5500, gateway, subnet, src, mac);
  w5500_socket_init(w5500, s0, s0_dst, s0_src_port, s0_dst_port);

  w5500_status(w5500, s0);
  ip_t new_dst = {5,6,7,8};
  w5500_set_dst(w5500, s0, new_dst );
  w5500_status(w5500, s0);
  while (true) {
    
    sleep_ms(500);
    
    // w5500_rw(w5500, w5500_mr, cmn, &data, 1, false);
    // printf("0x%x\n", data);

  }
}