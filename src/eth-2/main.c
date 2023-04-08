
#include <pico/stdlib.h>
#include <stdio.h>
#include <w5500.h>
#include <hardware/clocks.h>

SPI_DEVICE(w5500, spi0, 17);
int main() {
  // set_sys_clock_khz(160000, true);
  stdio_init_all();

  spi_init(spi0, 1000 * 1000);
  gpio_set_function(16, GPIO_FUNC_SPI);
  gpio_set_function(18, GPIO_FUNC_SPI);
  gpio_set_function(19, GPIO_FUNC_SPI);

  for (int i = 0; i < 10; i++) {
    printf("debug point 0 %d\n", i);
    sleep_ms(1000);
  }

  uint actual_baud = w5500_set(w5500);
  ip_t gateway = {192,168,2,50};
  ip_t subnet = {0xff, 0xff, 0xff, 0x00};
  ip_t src = {192, 168, 2, 102};
  mac_t mac = {0x08, 0xDC, 0x0A, 0x01, 0x10, 0x18};
  ip_t dst = {192,168,2,50};
  mac_t dhar = {0x09, 0x10, 0x11, 0x12, 0x13, 0x14};
  uint16_t src_port = 5000;
  uint16_t dst_port = 5353;
  uint8_t data[256];

  for(int i = 0; i < 256; i++) {
data[i] = i;
  }    

  printf("debug point 1, actual baud %d\n", actual_baud);
  w5500_init(w5500, gateway, subnet, src, mac, false, false, false);
  w5500_socket_init(w5500, s1, udp, src_port, dst, dst_port, 16, 16, false, false, false, dhar);
  w5500_cmd_connect(w5500, s1);
  w5500_cmd_send(w5500, s1);
  sleep_ms(1000);
  
  while (true) {
    w5500_write_tx(w5500, s1, data, 256);
    while(!w5500_cmd_send(w5500,  s1)){}
    }
}