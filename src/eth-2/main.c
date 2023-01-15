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
  ip_t gateway = {192,168,2,50};
  ip_t subnet = {0xff, 0xff, 0xff, 0x00};
  ip_t src = {192, 168, 2, 102};
  mac_t mac = {0x08, 0xDC, 0x0A, 0x01, 0x10, 0x18};
  ip_t dst = {192, 168, 2, 101};
  uint16_t src_port = 5000;
  uint16_t dst_port = 5000;
  uint8_t data[8] = {0x0A,0x0D,0x03,0x04, 0x00};
  printf("debug point 1, actual baud %d\n", actual_baud);
  w5500_init(w5500, gateway, subnet, src, mac);
  w5500_socket_init(w5500, s1, udp, src_port, dst, dst_port, 2, 2);
  w5500_print_all(w5500, s1);
  while (true) {
  
  w5500_write_tx(w5500, s1, data, 8);
  w5500_send(w5500, s1);
  sleep_ms(500);
  
  }
}