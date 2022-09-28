#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <max31856.h>
#include <pico/stdlib.h>
#include <spi_helpers.h>
#include <stdio.h>

#define DEFAULT_BAUD 1000000  // 1 MHz
#define tc_hw spi0

const uint tc_cs = 26;

int main() {
  spi_init(tc_hw, DEFAULT_BAUD);
  gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
  gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

  max31856_set(tc_hw);
  max31856_wreg_byte(tc_hw, max31856_cr0, max31856_cr0_default | max31856_cr0_cmode);  // build off of default cr0, enable continous mode

  while (true) {  // Main loop
    cs_select(tc_cs);
    int tc_temp   = max31856_get_tc_temp(tc_hw);
    short cj_temp = max31856_get_cj_temp(tc_hw);
    uint8_t fault = max31856_rreg_byte(tc_hw, max31856_sr);
    cs_deselect(tc_cs);

    printf("TC temp: %d, CJ temp: %d", tc_temp, cj_temp);  // temperature values

    // print fault reg as binary
    printf("Fault reg: ");
    for (int i = 7; i >= 0; i--) {
      printf("%d", (fault >> i) & 1);
    }
  }
}