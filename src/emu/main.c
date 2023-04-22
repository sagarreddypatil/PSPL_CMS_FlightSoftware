#include <pico/stdlib.h>
#include <pico/unique_id.h>
#include <stdio.h>
#include <w5500.h>
#include <commandnet.h>

SPI_DEVICE(w5500, spi0, 17);

//------------Network Constants------------
ip_t gateway     = {192, 168, 2, 1};
ip_t subnet_mask = {255, 255, 255, 0};
ip_t src_ip      = {192, 168, 2, 50};

int main() {
  //------------All Initialization------------
  stdio_init_all();

  spi_init(spi0, 5000);
  gpio_set_function(18, GPIO_FUNC_SPI);
  gpio_set_function(19, GPIO_FUNC_SPI);
  gpio_set_function(16, GPIO_FUNC_SPI);

  w5500_set(w5500);
  w5500_init(w5500);

  pico_unique_board_id_t board_id;
  pico_get_unique_board_id(&board_id);

  mac_t src_mac = {0x02,           board_id.id[3], board_id.id[4],
                   board_id.id[5], board_id.id[6], board_id.id[7]};

  //------------Poll Initialization Complete------------
  while (!stdio_usb_connected()) tight_loop_contents();
  while (!w5500_ready(w5500)) tight_loop_contents();

  for (int i = 0; i < 10; i++) printf("\n");

  //------------Main Program Begin------------

  printf("Program: %s\n", PICO_PROGRAM_NAME);
  printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
  printf("\n=====Startup Information=====\n");
  printf("Took %llu µs\n", time_us_64());

  //------------Setup and Configuration------------
  // W5500 Configuration
  w5500_config(w5500, src_mac, src_ip, subnet_mask, gateway);
  ip_t ip;
  w5500_rw(w5500, W5500_COMMON, W5500_SIPR0, false, ip, sizeof(ip));
  printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", src_mac[0], src_mac[1],
         src_mac[2], src_mac[3], src_mac[4], src_mac[5]);
  printf("IP: %d.%d.%d.%d\n", src_ip[0], src_ip[1], src_ip[2], src_ip[3]);

  while (true) {
  }
}