#include <ntp.h>
#include <stdio.h>
#include <psp.h>
#include <pico/stdlib.h>

#include <w5500.h>

SPI_DEVICE(w5500, spi1, 15);

void run_ntp_test() {
  stdio_init_all();
  while (!stdio_usb_connected())
    ;

  for (int i = 0; i < 10; i++) {
    printf("\n");
  }

  printf(psp_logo);
  printf("Program: %s\n", PICO_PROGRAM_NAME);
  printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);

  spi_init_bus(spi1, 11, 12, 14);

  uint actual_baud = w5500_set(w5500);
  printf("actual baud: %d\n", actual_baud);

  ip_t gateway     = {192, 168, 2, 1};
  ip_t subnet_mask = {255, 255, 255, 0};
  ip_t src_ip      = {192, 168, 2, 50};
  mac_t src_mac    = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};

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

  ip_t dest = {192, 168, 2, 1};

  while (true) {
    ntp_resp_t response = get_server_time(w5500, dest, W5500_S3);
    printf("server time: %lld\n", response.server_us);
    printf("local time: %lld\n", response.local_us);
    printf("delay: %lld\n", response.local_us - response.server_us);
    sleep_ms(500);
  }
}