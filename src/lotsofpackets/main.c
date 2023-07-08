#include <stdio.h>
#include <pico/stdlib.h>

#include <w5500.h>

SPI_DEVICE(w5500, spi1, 13);

int main() {
  stdio_init_all();
  while (!stdio_usb_connected())
    ;

  for (int i = 0; i < 10; i++) {
    printf("Program: %s\n", PICO_PROGRAM_NAME);
    printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
  }

  spi_init(spi1, 5000);  // 5MHz
  gpio_set_function(11, GPIO_FUNC_SPI);
  gpio_set_function(12, GPIO_FUNC_SPI);
  gpio_set_function(14, GPIO_FUNC_SPI);

  uint actual_baud = w5500_set(w5500);
  printf("actual baud: %d\n", actual_baud);

  ip_t gateway     = {192, 168, 1, 1};
  ip_t subnet_mask = {255, 255, 255, 0};
  ip_t src_ip      = {192, 168, 1, 50};
  mac_t src_mac    = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};

  w5500_reset(w5500);
  uint64_t start = time_us_64();
  while (!w5500_ready(w5500))
    ;
  printf("W5500 ready, awaiting link, took %d us\n", (int)(time_us_64() - start));

  while (!w5500_has_link(w5500))
    ;
  printf("W5500 has link, took %d us\n", (int)(time_us_64() - start));

  w5500_config(w5500, src_mac, src_ip, subnet_mask, gateway);

  // print ip
  ip_t ip;
  w5500_read(w5500, W5500_COMMON, W5500_SIPR0, ip, sizeof(ip));
  printf("Connected, IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

  ip_t victim          = {192, 168, 1, 4};
  uint16_t victim_port = 5000;
  w5500_create_udp_socket(w5500, W5500_S0, 5000, false, false, false);

  char msg[1024];
  for (int i = 0; i < 1024; i++) {
    msg[i] = 'A';
  }

  w5500_write(w5500, W5500_S0, W5500_Sn_DIPR0, victim, 4);
  w5500_write16(w5500, W5500_S0, W5500_Sn_DPORT0, victim_port);

  while (true) {
    // blast UDP packets to victim
    w5500_write_data(w5500, W5500_S0, msg, sizeof(msg));
  }
}
