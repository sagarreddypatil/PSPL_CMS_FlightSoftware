#pragma once

#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <pico/stdlib.h>

static inline void cs_init(uint cs_pin) {
  gpio_init(cs_pin);
  gpio_put(cs_pin, 1);
  gpio_set_dir(cs_pin, GPIO_OUT);
}

static inline void cs_select(uint cs_pin) {
  asm volatile("nop \n nop \n nop");  // from pico examples, dunno why but monkey see monkey do
  gpio_put(cs_pin, 0);
  asm volatile("nop \n nop \n nop");
}

static void cs_deselect(uint cs_pin) {
  asm volatile("nop \n nop \n nop");
  gpio_put(cs_pin, 1);
  asm volatile("nop \n nop \n nop");
}