#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <max31856.h>
#include <pico/stdlib.h>
#include <spi_helpers.h>
#include <stdio.h>

#define DEFAULT_BAUD 1000000  // 1 MHz
#define tc_hw spi0

const uint tc_cs = 20;

int main() {
  stdio_init_all();

  spi_init(tc_hw, DEFAULT_BAUD);
  gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);   // TX
  gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);  // SCK
  gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);   // RX

  int actual_baud = (int)max31856_set(tc_hw);

  // max31856_wreg_byte(tc_hw, max31856_cr0, max31856_cr0_default | max31856_cr0_cmode);  // build off of default cr0, enable continous mode

  // set builtin LED to output
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  gpio_put(PICO_DEFAULT_LED_PIN, 1);

  cs_init(tc_cs);
  while (true) {       // Main loop
    uint8_t data[17];  // for reading all the registers

    cs_select(tc_cs);
    spi_read_blocking(tc_hw, 0, data, 17);
    cs_deselect(tc_cs);

    // print all values, one per line
    printf("actual baud: %d\n", actual_baud);
    for (int i = 1; i < 16; i++) {
      printf("%02x\n", data[i]);
    }

    sleep_ms(100);
    // gpio_put(PICO_DEFAULT_LED_PIN, 1);
    sleep_ms(100);
    // gpio_put(PICO_DEFAULT_LED_PIN, 0);
  }
}