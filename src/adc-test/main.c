#include <pico/stdlib.h>
#include <stdio.h>
#include <ads13x.h>

#define ADC_SCLK 10
#define ADC_PICO 11
#define ADC_POCI 12
#define ADC_CS 13

SPI_DEVICE(ads13x, spi1, ADC_CS)

int main() {
  stdio_init_all();
  while (!stdio_usb_connected()) tight_loop_contents();

  for (int i = 0; i < 50; i++) printf("\n");

  printf("Program: %s\n", PICO_PROGRAM_NAME);
  printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
  printf("\n=====Startup Information=====\n");
  printf("Took %llu µs\n", time_us_64());

  gpio_set_function(ADC_SCLK, GPIO_FUNC_SPI);
  gpio_set_function(ADC_PICO, GPIO_FUNC_SPI);
  gpio_set_function(ADC_POCI, GPIO_FUNC_SPI);
  spi_init(spi1, 100000);

  printf("Initializing ADS13x...\n");
  ads13x_set(ads13x);
  ads13x_reset(ads13x);
  while (!ads13x_ready(ads13x)) tight_loop_contents();
  ads13x_init(ads13x);
  printf("ADS13x Init\n");

  while (true) {
    // print all regs
    uint16_t id     = ads13x_rreg_single(ads13x, ads13x_id);
    uint16_t status = ads13x_rreg_single(ads13x, ads13x_status);
    uint16_t mode   = ads13x_rreg_single(ads13x, ads13x_mode);
    uint16_t clock  = ads13x_rreg_single(ads13x, ads13x_clock);
    uint16_t gain   = ads13x_rreg_single(ads13x, ads13x_gain);
    uint16_t cfg    = ads13x_rreg_single(ads13x, ads13x_cfg);

    printf("ID:     %04x\n", id);
    printf("Status: %04x\n", status);
    printf("Mode:   %04x\n", mode);
    printf("Clock:  %04x\n", clock);
    printf("Gain:   %04x\n", gain);
    printf("Cfg:    %04x\n", cfg);

    sleep_ms(100);
    continue;

    // uint16_t status;
    // uint32_t data[2];
    // ads13x_read_data(ads13x, &status, data, 2);

    // printf("Status: %04x\n", status);
    // printf("Data: %lu %lu\n", data[0], data[1]);
  }
}