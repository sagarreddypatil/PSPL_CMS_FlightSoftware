#include <spi.h>
#include <pico/binary_info.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"


spi_device_t test_device = {
  .spi_inst = spi0,
  .miso_gpio = 0,
  .mosi_gpio = 0,
  .sck_gpio = 0,
  .cs_gpio = 0,
  .baudrate = 10000
  };

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected())
    ;
    printf("\n\nstdio connected!\n");

    // spi_device_init(&test_device);

    uint8_t tx_src[4] = {1,2,3,4};
    uint8_t tx_dst[4] = {0,0,0,0};  // SPI FIFO (TX)
    uint8_t rx_src[4] = {5,6,7,8};  // SPI FIFO (RX)
    uint8_t rx_dst[4] = {0,0,0,0};

    dma_channel_configure(
		test_device.tx_dma,
		&test_device.tx_dma_config,
		tx_dst,
		tx_src,
		4,
		false 
	);
    dma_channel_configure(
		test_device.rx_dma,
		&test_device.rx_dma_config,
		rx_dst,
		rx_src,
		4,
		false 
	);

    printf("DMA configured!\n");

    dma_move(tx_src, tx_dst, sizeof(tx_src));

    printf("TX DST: ");
    for (size_t i = 0; i < 4; i++)
    {
        printf("0x%x ", tx_dst[i]);
    }
    printf("RX DST: ");
    for (size_t i = 0; i < 4; i++)
    {
        printf("0x%x ", rx_dst[i]);
    }

    while(1);
    
    return 0;
}