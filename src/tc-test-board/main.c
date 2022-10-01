#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <max31856.h>
#include <pico/stdlib.h>
#include <stdio.h>

#define DEFAULT_BAUD 1000000  // 1 MHz

SPI_DEVICE(tc_spi, spi0, 20)

int main() {
  stdio_init_all();

  spi_init(spi0, DEFAULT_BAUD);
  gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);   // TX
  gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);  // SCK
  gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);   // RX

  int actual_baud = (int)max31856_set(tc_spi);

  for (int i = 0; i < 100; i++) {
    max31856_wreg_byte(tc_spi, max31856_cr0, max31856_cr0_default | max31856_cr0_cmode);  // build off of default cr0, enable continous mode
  }

  // set builtin LED to output
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  gpio_put(PICO_DEFAULT_LED_PIN, 1);

  while (true) {  // Main loop
    // uint8_t data[17];  // for reading all the registers

    uint8_t cr0   = max31856_rreg_byte(tc_spi, max31856_cr0);
    uint8_t cr1   = max31856_rreg_byte(tc_spi, max31856_cr1);
    int tc_temp   = max31856_get_tc_temp(tc_spi);
    short cj_temp = max31856_get_cj_temp(tc_spi);
    uint8_t sr    = max31856_rreg_byte(tc_spi, max31856_sr);

    float tc_temp_f = tc_temp * 0.0078125;
    float cj_temp_f = cj_temp * 0.00390625;

    // print all values, one per line
    printf("the actual baud: %d\n", actual_baud);
    printf("cr0: 0x%02x\n", cr0);
    printf("cr1: 0x%02x\n", cr1);
    // printf("tc_temp: %d\n", tc_temp);
    // printf("cj_temp: %d\n", cj_temp);
    printf("tc_temp: %f deg C\n", tc_temp_f);
    printf("cj_temp: %f deg C\n", cj_temp_f);
    printf("sr: 0x%02x\n", sr);

    sleep_ms(100);
    gpio_put(PICO_DEFAULT_LED_PIN, 1);
    sleep_ms(100);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
  }
}