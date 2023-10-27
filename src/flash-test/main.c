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
  
  long i = 0;
  while (true) {
    w25n01_read_page(flash, i); 
    
  }
}
