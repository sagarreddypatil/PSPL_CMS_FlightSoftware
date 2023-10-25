#include <stdio.h>
#include <pico/stdlib.h>
#include <w25n01.h>

SPI_DEVICE(flash, spi1, 15);

int main() {
  stdio_init_all();
  while (!stdio_usb_connected())
    ;

  spi_init_bus(spi1, 26, 27, 28);
  w25n01_set(flash);

  printf("Flash Test Program Start\n");

  while (true) {
    w25n01_jedec_id_t id = w25n01_read_jedec_id(flash);

    printf("mfr id: %x\tdevice id: %x %x\n", id.mfrId, id.deviceId > 8,
           id.deviceId & 0xFF);
    sleep_ms(500);
  }
}