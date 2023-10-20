#include <ntp.h>
#include <stdio.h>
#include <psp.h>
#include <pico/stdlib.h>

#include <w5500.h>

SPI_DEVICE(w5500, spi0, 17);

int main() {
  stdio_init_all();
  while (!stdio_usb_connected())
    ;

  for (int i = 0; i < 10; i++) {
    printf("\n");
  }

  printf(psp_logo);
  printf("Program: %s\n", PICO_PROGRAM_NAME);
  printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);

  spi_init_bus(spi0, 19, 18, 16);

  uint actual_baud = w5500_set(w5500);
  printf("actual baud: %d\n", actual_baud);

  ip_t gateway       = {192, 168, 2, 1};
  ip_t ntp_server_ip = {192, 168, 2, 1};
  ip_t subnet_mask   = {255, 255, 255, 0};
  ip_t src_ip        = {192, 168, 2, 50};
  mac_t src_mac      = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};

  w5500_reset(w5500);
  uint64_t start = time_us_64();
  while (!w5500_ready(w5500))
    ;
  printf("W5500 ready, awaiting link, took %d us\n",
         (int)(time_us_64() - start));

  while (!w5500_has_link(w5500))
    ;
  printf("W5500 has link, took %d us\n", (int)(time_us_64() - start));

  w5500_config(w5500, src_mac, src_ip, subnet_mask, gateway);

  while (true) {
    ntp_resp_t response = get_server_time(w5500, ntp_server_ip, W5500_S3);

    uint64_t offset = response.server_us - response.local_us;
    // printf("server time us: %lld\n", response.server_us);
    // printf("local time us: %lld\n", response.local_us);

    printf("%lld\n", offset);

    sleep_ms(10);
  }
}
