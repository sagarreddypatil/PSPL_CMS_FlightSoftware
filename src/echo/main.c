
#include <pico/stdlib.h>
#include <stdio.h>
#include <w5500.h>

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
  w5500_rw(w5500, W5500_COMMON, W5500_SIPR0, false, ip, sizeof(ip));
  printf("IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

  w5500_create_tcp_socket(w5500, W5500_S0, 8080);

  while (true) {
    uint8_t status = w5500_read8(w5500, W5500_S0, W5500_Sn_SR);

    if (status == W5500_SOCK_CLOSED) {
      printf("Socket closed, reopening\n");
      w5500_command(w5500, W5500_S0, W5500_CMD_OPEN);
    }

    if (status == W5500_SOCK_INIT) {
      printf("Socket init, listening\n");
      w5500_command(w5500, W5500_S0, W5500_CMD_LISTEN);
    }

    if (status == W5500_SOCK_CLOSE_WAIT) {
      printf("Socket close wait, disconnecting\n");
      w5500_command(w5500, W5500_S0, W5500_CMD_DISCON);
    }

    if (status == W5500_SOCK_ESTABLISHED) {
      printf("Socket established\n");
      while ((status = w5500_read8(w5500, W5500_S0, W5500_Sn_SR)) == W5500_SOCK_ESTABLISHED) {
        uint16_t rx_size = w5500_read16(w5500, W5500_S0, W5500_Sn_RX_RSR0);
        if (rx_size > 0) {
          uint8_t data[rx_size + 1];
          w5500_read_data(w5500, W5500_S0, data, rx_size);
          data[rx_size] = 0;

          printf("Read from client: %s\n", data);

          w5500_write_data(w5500, W5500_S0, false, data, rx_size);
        }
      }
    }
  }
}