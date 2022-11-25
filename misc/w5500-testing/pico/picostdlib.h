
#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define GPIO_OUT 1
#define GPIO_IN 0

void gpio_init(uint pin);
void gpio_put(uint pin, bool value);
void gpio_set_dir(uint pin, bool out);
