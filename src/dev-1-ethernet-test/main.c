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
  // uint8_t recv[6];
  // uint8_t s_mr = 0x82;
  // uint8_t s_cr = 0x01;
  // uint8_t mr = 0xA8;
  ip_t gateway = {0x02, 0x06, 0x07, 0x08};
  ip_t subnet = {0xff, 0xff, 0x00, 0x00};
  ip_t src = {192, 168, 2, 1};
  mac_t mac = {0x08, 0xDC, 0x0A, 0x01, 0x10, 0x18};
  // uint8_t dport[2] = {0x13, 0x88};
 
  printf("debug point 1, actual baud %d\n", actual_baud);
  // w5500_rw(w5500, w5500_mr, cmn, &mr, 1, true);
  // w5500_rw(w5500, w5500_s_mr, s1, &s_mr, 1, true);
  // w5500_rw(w5500, w5500_s_cr, s1, &s_cr, 1, true);
  // w5500_rw(w5500, w5500_s_sport0, s1, dport, 2, true);
  // w5500_rw(w5500, w5500_s_dport0, s1, dport, 2, true);
  // w5500_rw(w5500, w5500_s_dipr0, s1, dipr, 4, true);
  // s_cr = 0x01;
  // w5500_rw(w5500, w5500_s_cr, s1, &s_cr, 1, true);
  // s_cr = 0x10;
  // w5500_rw(w5500, w5500_s_cr, s1, &s_cr, 1, true);
  // dipr[0] = 0x11;
  // w5500_rw(w5500, w5500_s_dipr0, s1, dipr, 4, true);
  // s_cr = 0x01;
  // w5500_rw(w5500, w5500_s_cr, s1, &s_cr, 1, true);
  w5500_init(w5500, gateway, subnet, src, mac);

  while (true) {
    w5500_status(w5500);
    sleep_ms(500);
    
    // w5500_rw(w5500, w5500_mr, cmn, &data, 1, false);
    // printf("0x%x\n", data);

  }
}