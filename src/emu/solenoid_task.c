#include "emu.h"

int64_t eth_cycle_start = 0;
int64_t lox_cycle_start = 0;

void solenoid_task_init() {
  gpio_init(LOX_SOLENOID);
  gpio_init(ETH_SOLENOID);

  gpio_put(LOX_SOLENOID, 0);
  gpio_put(ETH_SOLENOID, 0);

  gpio_set_dir(LOX_SOLENOID, GPIO_OUT);
  gpio_set_dir(ETH_SOLENOID, GPIO_OUT);
}

void solenoid_task_run() {
  int64_t solenoid_time = time_us_64();
  int64_t on_time       = eth_period * eth_duty_cycle / 100000;

  if (eth_run) {
    if (eth_cycle_start + eth_period <= solenoid_time) {
      eth_cycle_start = solenoid_time;
    }

    if (eth_cycle_start + on_time <= solenoid_time) {
      gpio_put(ETH_SOLENOID, 0);
    } else {
      gpio_put(ETH_SOLENOID, 1);
    }
  } else {
    gpio_put(ETH_SOLENOID, 0);
  }

  solenoid_time = time_us_64();
  on_time       = lox_period * lox_duty_cycle / 100000;

  if (lox_run) {
    if (lox_cycle_start + lox_period <= solenoid_time) {
      lox_cycle_start = solenoid_time;
    }

    if (lox_cycle_start + on_time <= solenoid_time) {
      gpio_put(LOX_SOLENOID, 0);
    } else {
      gpio_put(LOX_SOLENOID, 1);
    }
  } else {
    gpio_put(LOX_SOLENOID, 0);
  }
}