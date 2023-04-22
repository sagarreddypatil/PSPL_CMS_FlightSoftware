#include <stdio.h>
#include <pico/stdlib.h>

#include <w5500.h>
#include <w5500/tcp_server.h>

SPI_DEVICE(w5500, spi0, 17);

int main() {
  stdio_init_all();
  while (!stdio_usb_connected())
    ;

  for (int i = 0; i < 10; i++) {
    printf("Program: %s\n", PICO_PROGRAM_NAME);
    printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
  }

  spi_init(spi0, 5000 * 1000);  // 5MHz
  gpio_set_function(18, GPIO_FUNC_SPI);
  gpio_set_function(19, GPIO_FUNC_SPI);
  gpio_set_function(16, GPIO_FUNC_SPI);

  uint actual_baud = w5500_set(w5500);
  printf("actual baud: %d\n", actual_baud);

  ip_t gateway     = {192, 168, 2, 1};
  ip_t subnet_mask = {255, 255, 255, 0};
  ip_t src_ip      = {192, 168, 2, 50};
  mac_t src_mac    = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};

  w5500_init(w5500);
  uint64_t start = time_us_64();
  while (!w5500_ready(w5500))
    ;
  printf("W5500 ready, took %d us\n", (int)(time_us_64() - start));

  w5500_config(w5500, src_mac, src_ip, subnet_mask, gateway);

  // print ip
  ip_t ip;
  w5500_read(w5500, W5500_COMMON, W5500_SIPR0, ip, sizeof(ip));
  printf("Connected, IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

  uint16_t avail      = 0;
  bool prev_connected = false;
  bool connected      = false;
  tcp_client_data_t client;
  tcp_server_t server;
  tcp_server_init(&server, w5500, W5500_S0, 8080);

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
}