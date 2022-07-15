#ifndef BME280_H
#define BME280_H

#include "spi.h"

#define HUMIDITY    0xFD
#define PRESSURE    0xF7
#define TEMPERATURE 0xFA
#define BME_SS      

uint16_t bme280_humidity();
uint32_t bme280_temeperature();
uint32_t bme280_pressure();

#endif