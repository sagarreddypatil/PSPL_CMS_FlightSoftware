#include <w5500.h>
#include <w5500/tcp_server.h>
#include <spi.h>
#include <pico/binary_info.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <stdio.h>

spi_device_t w5500 = {
  .spi_inst = spi0,
  .miso_gpio = 11,
  .mosi_gpio = 12,
  .sck_gpio = 14,
  .cs_gpio = 15, // CS_0
  .baudrate = 4000000 // 4MHz
  };

void spi0_dma_isr() { spi_irq_handler(&w5500); }

void echo_main() {

  spi_device_init(&w5500);

#if PICOTOOL_ENABLED
	bi_decl(bi_3pins_with_func(11, 12, 14, GPIO_FUNC_SPI));
	bi_decl(bi_1pin_with_name(15, "SPI CS"));

  hw_set_bits(&spi_get_hw(spi1)->cr1, SPI_SSPCR1_LBM_BITS);
#endif

  stdio_init_all();
  while (!stdio_usb_connected())
    ; // @todo timeout needed


  printf("Program: %s\n", PICO_PROGRAM_NAME);
  printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);


  irq_set_exclusive_handler(DMA_IRQ_0, spi0_dma_isr);
  irq_set_priority(DMA_IRQ_0, 0xFF);  // Lowest urgency.

  // Tell the DMA to raise IRQ line 0 when the channel finishes a block
  dma_channel_set_irq0_enabled(w5500.rx_dma, true);
  irq_set_enabled(DMA_IRQ_0, true);

  printf("actual baud: %d\n", w5500.baudrate);

  ip_t gateway     = {192, 168, 1, 1};
  ip_t subnet_mask = {255, 255, 255, 0};
  ip_t src_ip      = {192, 168, 1, 50};
  mac_t src_mac    = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};

  printf("waiting on reset");
  w5500_reset(&w5500);
  uint64_t start = time_us_64();

  printf("readying w5500");
  while (!w5500_ready(&w5500))
    ; // @todo timeout needed
  printf("W5500 ready, took %d us\n", (int)(time_us_64() - start));

  while (!w5500_has_link(&w5500))
    ; // @todo timeout needed
  printf("W5500 has link, took %d us\n", (int)(time_us_64() - start));

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
}