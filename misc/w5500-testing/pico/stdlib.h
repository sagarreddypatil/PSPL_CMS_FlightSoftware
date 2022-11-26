
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define GPIO_OUT 1
#define GPIO_IN 0

void gpio_init(unsigned int pin);
void gpio_put(unsigned int pin, bool value);
void gpio_set_dir(unsigned int pin, bool out);
