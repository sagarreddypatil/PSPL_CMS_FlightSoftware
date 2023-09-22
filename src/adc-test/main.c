#include <pico/stdlib.h>
#include <stdio.h>
#include <ads13x.h>

#define ADC_SCLK 14
#define ADC_PICO 11
#define ADC_POCI 12
#define ADC_CS 15
#define ADC_RST 3

SPI_DEVICE(ads13x, spi1, ADC_CS)

static const int32_t ADC_MAX = 0x7FFFFF;
static const int32_t ADC_MIN = -0x800000;

int main() {
  gpio_init(ADC_RST);
  gpio_set_dir(ADC_RST, GPIO_OUT);
  gpio_put(ADC_RST, 1);
  stdio_init_all();
  while (!stdio_usb_connected()) tight_loop_contents();

  // for (int i = 0; i < 15; i++) {
  //   printf("iter %d, time %llu\n", i, time_us_64());
  // }
  // printf("\n");

  // while (true)
  //   ;

  for (int i = 0; i < 50; i++) printf("\n");

  printf("Program: %s\n", PICO_PROGRAM_NAME);
  printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
  printf("\n=====Startup Information=====\n");
  printf("Took %llu µs\n", time_us_64());

  gpio_set_function(ADC_SCLK, GPIO_FUNC_SPI);
  gpio_set_function(ADC_PICO, GPIO_FUNC_SPI);
  gpio_set_function(ADC_POCI, GPIO_FUNC_SPI);
  spi_init(spi1, 2 * 1000 * 1000);

  printf("Initializing ADS13x...\n");
  uint baud = ads13x_set(ads13x);
  ads13x_reset(ads13x);
  while (!ads13x_ready(ads13x)) tight_loop_contents();
  ads13x_init(ads13x);
  printf("ADC Baud: %u\n", baud);

  printf("chan1,chan2\n");
  while (true) {
    // // print all regs
    // uint16_t id     = ads13x_rreg_single(ads13x, ads13x_id);
    // uint16_t status = ads13x_rreg_single(ads13x, ads13x_status);
    // uint16_t mode   = ads13x_rreg_single(ads13x, ads13x_mode);
    // uint16_t clock  = ads13x_rreg_single(ads13x, ads13x_clock);
    // uint16_t gain   = ads13x_rreg_single(ads13x, ads13x_gain);
    // uint16_t cfg    = ads13x_rreg_single(ads13x, ads13x_cfg);

    // printf("ID:     %04x\n", id);
    // printf("Status: %04x\n", status);
    // printf("Mode:   %04x\n", mode);
    // printf("Clock:  %04x\n", clock);
    // printf("Gain:   %04x\n", gain);
    // printf("Cfg:    %04x\n", cfg);

    // sleep_ms(100);

    uint16_t statA;
    // uint16_t statB;
    int32_t data[2];
    ads13x_read_data(ads13x, &statA, data, 2);

    float fdata[2] = {1.2 * data[0] / (float)ADC_MAX,
                      1.2 * data[1] / (float)ADC_MAX};
    // printf("Stat: %04x, A: %ld, B: %ld\n", statA, data[0], data[1]);
    // printf("%ld, %ld\n", data[0], data[1]);
    printf("%f,%f\n", fdata[0], fdata[1]);
  }
}
