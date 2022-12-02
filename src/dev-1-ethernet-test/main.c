#include <pico/stdlib.h>
#include <stdio.h>
#include <w5500.h>

SPI_DEVICE(w5500, spi0, 0);
int main() {
  stdio_init_all();

  int data;
  for (int i = 0; i < 10; i++) {
    printf("debug point 0 %d\n", i);
    sleep_ms(1000);
  }

  w5500_set(w5500);

  printf("debug point 1\n");

  while (true) {
    printf("debug point 2\n");
    w5500_rw(w5500, w5500_versionr, cmn, &data, 1, false);
    printf("0x%x\n", data);

    // w5500_configMR(w5500, 1, 1, 1, 1);
    // w5500_rw(w5500, w5500_mr, cmn, &data, 1, false);
    // printf("0x%x\n", data);

    sleep_ms(1000);
  }
}