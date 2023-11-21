#include <ntp.h>
#include <stdio.h>
#include <psp.h>
#include <pico/stdlib.h>
#include <w5500.h>

spi_device_t w5500 = { //posi pico sclk
  .spi_inst = spi1,
  .miso_gpio = 27,
  .mosi_gpio = 26,
  .sck_gpio = 28,
  .cs_gpio = 25,
  .baudrate = 30000000
  };

int64_t offset = 0;

static inline uint64_t unix_time_us() {
  return time_us_64() + offset;
}

int main() {

  spi_device_init(&w5500);

  stdio_init_all();
  while (!stdio_usb_connected())
    ;

  for (int i = 0; i < 10; i++) {
    printf("\n");
  }

  printf(psp_logo);
  printf("Program: %s\n", PICO_PROGRAM_NAME);
  printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);

  printf("actual baud: %d\n", w5500.baudrate);

  ip_t gateway       = {192, 168, 2, 1};
  ip_t ntp_server_ip = {192, 168, 2, 1};
  ip_t subnet_mask   = {255, 255, 255, 0};
  ip_t src_ip        = {192, 168, 2, 50};
  mac_t src_mac      = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};

  w5500_reset(&w5500);
  uint64_t start = time_us_64();
  while (!w5500_ready(&w5500))
    ;
  printf("W5500 ready, awaiting link, took %d us\n",
         (int)(time_us_64() - start));

  while (!w5500_has_link(&w5500))
  {
    sleep_ms(2000);
  }
  printf("W5500 has link, took %d us\n", (int)(time_us_64() - start));

  w5500_config(&w5500, src_mac, src_ip, subnet_mask, gateway);
  printf("Config set.");
  while (true) {
    int64_t new_offset = get_server_time(&w5500, ntp_server_ip, W5500_S3);
    if (new_offset > 0) {
      offset = new_offset;
    }

    printf("%lld, %lld", unix_time_us(), offset);

    sleep_ms(100);
  }
}
