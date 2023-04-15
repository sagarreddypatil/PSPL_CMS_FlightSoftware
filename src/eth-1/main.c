
#include <pico/stdlib.h>
#include <stdio.h>
#include <w5500.h>

SPI_DEVICE(w5500, spi0, 0);
int main() {
  stdio_init_all();

  spi_init(spi0, 1000);
  gpio_set_function(2, GPIO_FUNC_SPI);
  gpio_set_function(3, GPIO_FUNC_SPI);
  gpio_set_function(4, GPIO_FUNC_SPI);

  uint actual_baud = w5500_set(w5500);
  printf("actual baud: %d\n", actual_baud);

  ip_t gateway     = {192, 168, 2, 50};
  ip_t subnet_mask = {0xff, 0xff, 0xff, 0x00};
  ip_t src_ip      = {192, 168, 2, 101};
  mac_t src_mac    = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};

  w5500_init(w5500, src_mac, src_ip, subnet_mask, gateway);
  w5500_create_tcp_socket(w5500, W5500_S0, 8080);

  while (true) {
    uint8_t status = w5500_read8(w5500, W5500_S0, W5500_Sn_SR);

    if (status == W5500_SOCK_CLOSED)
      w5500_command(w5500, W5500_S0, W5500_CMD_OPEN);

    if (status == W5500_SOCK_INIT)
      w5500_command(w5500, W5500_S0, W5500_CMD_LISTEN);

    if (status == W5500_SOCK_CLOSE_WAIT)
      w5500_command(w5500, W5500_S0, W5500_CMD_DISCON);

    if (status == W5500_SOCK_ESTABLISHED) {
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