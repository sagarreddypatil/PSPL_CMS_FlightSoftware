#include <stdio.h>
#include <pico/stdlib.h>
#include <w5500.h>
#include <spi.h>
#include <hardware/spi.h>

spi_device_t w5500 = {  // posi pico sclk
    .spi_inst = spi1, .miso_gpio = 27, .mosi_gpio = 26,
    .sck_gpio = 28,   .cs_gpio = 25,   .baudrate = 30000000};

int main() {
  spi_device_init(&w5500);

  stdio_init_all();
  while (!stdio_usb_connected())
    ;

  ip_t gateway     = {192, 168, 2, 1};
  ip_t subnet_mask = {255, 255, 255, 0};
  ip_t src_ip      = {192, 168, 2, 50};
  mac_t src_mac    = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};

  w5500_reset(&w5500);

  uint64_t start = time_us_64();
  uint count     = 0;
  do {
    count++;
  } while (!w5500_ready(&w5500));
  printf("W5500 ready, took %d us after %d tries\n",
         (int)(time_us_64() - start), count);

  count = 0;
  do {
    count++;
    sleep_ms(2000);
  } while (!w5500_has_link(&w5500));
  printf("W5500 has link, took %d us after %d tries\n",
         (int)(time_us_64() - start), count);

  w5500_config(&w5500, src_mac, src_ip, subnet_mask, gateway);

  while (true) {
    // reading free size registers (should return 0x800 or 2048 in decimal
    // because that is the max space and nothing has yet been written)
    uint16_t a = w5500_read16(&w5500, W5500_S1, 0x20);

    printf("0x%x\n", a);
    sleep_ms(500);

    // verifying correct IP was used (should match src_ip)
    ip_t ip;
    w5500_read(&w5500, W5500_COMMON, W5500_SIPR0, ip, sizeof(ip));
    printf("Connected, IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
  }

  return 0;
}