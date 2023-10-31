#include <w5500.h>
#include <w5500/tcp_server.h>
#include <spi.h>
#include <pico/binary_info.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

spi_device_t w5500 = {
  .spi_inst = spi0,
  .miso_gpio = 18,
  .mosi_gpio = 19,
  .sck_gpio = 16,
  .cs_gpio = 17,
  .baudrate = 9600
  };

int main() {

  spi_device_init(&w5500);

  stdio_init_all();
  while (!stdio_usb_connected())
    ; // @todo timeout needed

  printf("Program: %s\n", PICO_PROGRAM_NAME);
  printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);

  printf("Actual baud: %d\n", w5500.baudrate);

  ip_t gateway     = {192, 168, 2, 1};
  ip_t subnet_mask = {255, 255, 255, 0};
  ip_t src_ip      = {192, 168, 2, 50};
  mac_t src_mac    = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};

  w5500_reset(&w5500);
  printf("Reset complete.\n");
  uint64_t start = time_us_64();

  printf("Readying w5500...\n");
  uint count = 0;
  if(!w5500_ready(&w5500)){count++;} // @todo timeout needed
  printf("W5500 ready, took %d us after %d tries\n", (int)(time_us_64() - start), count);

  count = 0;
  while (!w5500_has_link(&w5500)){count++;} // @todo timeout needed
  printf("W5500 has link, took %d us after %d tries\n", (int)(time_us_64() - start), count);

  w5500_config(&w5500, src_mac, src_ip, subnet_mask, gateway);

  // print ip
  ip_t ip;
  w5500_read(&w5500, W5500_COMMON, W5500_SIPR0, ip, sizeof(ip));
  printf("Connected, IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

  while (true){
    uint8_t phyreg = w5500_read8(&w5500, W5500_COMMON, W5500_PHYCFGR);
    printf("0x%x\n", phyreg);
    sleep_ms(500);
  }

  uint16_t avail      = 0;
  bool prev_connected = false;
  bool connected      = false;
  tcp_client_data_t client;
  tcp_server_t server;
  tcp_server_init(&server, &w5500, W5500_S0, 8080);

  while (true) {
    tcp_server_poll(&server);

    connected = tcp_server_connected(&server);
    if (!prev_connected && connected) {
      client = tcp_server_get_client(&server);
      printf("Client connected: %d.%d.%d.%d:%d\n", client.ip[0], client.ip[1],
             client.ip[2], client.ip[3], client.port);
    }

    if (prev_connected && !connected) {
      printf("Client disconnected\n");
    }

    prev_connected = connected;

    if (connected && (avail = tcp_server_available(&server)) > 0) {
      uint8_t data[avail + 1];
      tcp_server_read(&server, data, avail);
      data[avail] = 0;

      printf("Received: %s\n", data);

      tcp_server_send(&server, data, avail);
    }
  }
  return 0;
}