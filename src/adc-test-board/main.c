//
// Created by Mihir Patil on 12/1/22.
//

#include <ads131x.h>
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <pico/stdlib.h>
#include <stdio.h>

SPI_DEVICE(adc, spi0, 7)

int main() {
  stdio_init_all();
  spi_init(spi0, 1000000);

  sleep_ms(5000);
  gpio_set_function(2, GPIO_FUNC_SPI);
  gpio_set_function(3, GPIO_FUNC_SPI);
  gpio_set_function(4, GPIO_FUNC_SPI);

  int baud_rate = (int)ads131x_set(adc);

  printf("the actual baud: %d\n", baud_rate);

  printf("ran set!\n");

  printf("ran init!\n");
  //  uint8_t id = ads131x_rreg_single(adc,ads131x_id);
  //  printf("id reg %d\n", id);
  //
  //  uint8_t status = ads131x_rreg_single(adc,ads131x_status);
  //  printf("status reg %d\n", status);
  //
  //  uint16_t data = 0;
  //
  clock_gpio_init(21, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_SYS, 61);
  while (true) {
    //    status = ads131x_rreg_single(adc,ads131x_status);
    //    printf("status reg %d\n", status);
//    ads131x_rreg_single(adc, ads131x_mode);
      ads131x_read_data(adc);
//        printf("ADC Data: %lu\n", data);
  }
}
