#include <psp.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/adc.h>

// GPIO
static const uint PYRO = 28;

// ADC Builtin
static const uint PYRO_CONT_0       = 26;
static const uint PYRO_CONT_0_INPUT = 0;
static const uint PYRO_CONT_1       = 27;
static const uint PYRO_CONT_1_INPUT = 1;

// States
uint PYRO_STATE = 0;

const uint64_t PYRO_SAMPLE_PERIOD = 10000;

void main() {
  stdio_init_all();
  while (!stdio_usb_connected())
    ;

  for (int i = 0; i < 10; i++) {
    printf("Program: %s\n", PICO_PROGRAM_NAME);
    printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
  }

  adc_init();
  adc_gpio_init(PYRO_CONT_0);
  adc_gpio_init(PYRO_CONT_1);

  uint64_t lastSampleTime = 0;
  while (1) {
    const uint64_t now = time_us_64();

    if (now - lastSampleTime > PYRO_SAMPLE_PERIOD) {
      lastSampleTime = now;

      adc_select_input(PYRO_CONT_0_INPUT);
      uint16_t pyro_cont_0 = adc_read();

      adc_select_input(PYRO_CONT_1_INPUT);
      uint16_t pyro_cont_1 = adc_read();

      printf("PYRO_CONT_0:\t%d\tPYRO_CONT_1:\t%d\tPYRO_SET_STATE:\t%d\n",
             pyro_cont_0, pyro_cont_1, PYRO_STATE);
    }

    char c;
    if (c = getchar_timeout_us(0)) {
      if (c == 'F') PYRO_STATE = !PYRO_STATE;
    }

    gpio_put(PYRO, PYRO_STATE);
  }
}
