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
  ip_t src = {192, 168, 2, 101};
  mac_t mac = {0x09, 0x10, 0x11, 0x12, 0x13, 0x14};
  ip_t dst = {192, 168, 2, 102};
  uint16_t src_port = 5000;
  uint16_t dst_port = 5000;
  uint8_t recv_buf[2000] = {0};
  printf("debug point 1, actual baud %d\n", actual_baud);
  w5500_init(w5500, gateway, subnet, src, mac);
  w5500_socket_init(w5500, s1, udp, src_port, dst, dst_port, 2 , 2);
  w5500_print_all(w5500, s1);
  while(true) {
    uint16_t recieved = w5500_recv(w5500, s1, recv_buf);
    for(int i = 0; i < recieved; i++) {
      printf(" 0x%x,", *(recv_buf + i));
    }
    printf("\n");
    sleep_ms(1000);
  }
  
}