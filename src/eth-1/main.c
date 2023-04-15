
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

  for (int i = 0; i < 10; i++) {
    printf("debug point 0 %d\n", i);
    sleep_ms(1000);
  }

  uint actual_baud = w5500_set(w5500);

  ip_t gateway = {192,168,2,50};
  ip_t subnet_mask = {0xff, 0xff, 0xff, 0x00};
  ip_t src_ip = {192,168,2,101};
  mac_t src_mac = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};
  //ip_t s1_dst = {192, 168, 2, 102};


  uint16_t src_port = 5000;
  //uint16_t dst_port = 5000;
  //uint8_t recv_buf[10] = {0};

  printf("debug point 1, actual baud %d\n", actual_baud);

  w5500_create(w5500, src_ip, src_mac, subnet_mask, gateway, 20, 5, false, false, false);

  //w5500_create_udp_socket(w5500, s1, src_port, false, false, false);

  w5500_create_tcp_server(w5500, s1, src_port);

  uint8_t recieved[50];

  w5500_read_data(w5500, s1, recieved);

while(1) {
  for(int i = 0; i < 50; i++) {
    printf("%x ", recieved[i]);
  }
  printf("\n");
  sleep_ms(500);
}


  w5500_print_all(w5500, s1);
  
}