#ifndef ADC_H
#define ADC_H

#include <stdlib.h>
#include <stdbool.h>
#include "spi.h"

spi_slave_t adc_init(volatile uint8_t *, volatile uint8_t *, uint8_t);
uint32_t adc_data(uint8_t);

#endif