//
// Created by Mihir Patil on 12/1/22.
//

#include <hardware/gpio.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <hardware/spi.h>


#ifdef PICO_DEFAULT_SPI_CSN_PIN
static inline void cs_select() {
  asm volatile("nop \n nop \n nop");
  printf("cs_select\n");
  gpio_put(7, 0);  // Active low
  asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
  asm volatile("nop \n nop \n nop");
  printf("cs_deselect\n");
  gpio_put(7, 1);
  asm volatile("nop \n nop \n nop");
}
#endif

int main() {
  stdio_init_all();

  spi_init(spi0, 10000);

  sleep_ms(5000);
  gpio_set_function(2, GPIO_FUNC_SPI);
  gpio_set_function(3, GPIO_FUNC_SPI);
  gpio_set_function(4, GPIO_FUNC_SPI);
  gpio_init(7);
  gpio_set_dir(7, GPIO_OUT);
  gpio_put(7, 1);


  sleep_ms(5000);
  printf("Starting write loop");
  uint8_t src[3] = {0b10100000,0b00000101,0};
  uint8_t dst[3];
  while (true) {
    cs_select();
    spi_write_read_blocking(spi0, src, dst, 3);
    for (int i = 0; i < 3; i++)
    {
        spi_read_blocking(spi0, 0, dst, 3);
        printf("%d %d %d", dst[0], dst[1], dst[2]);
    }
    cs_deselect();
  }
}