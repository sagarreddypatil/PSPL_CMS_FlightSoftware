#ifndef BME280_H
#define BME280_H

#include "spi.h"

#define HUMIDITY 0xFD
#define PRESSURE 0xF7
#define TEMPERATURE 0xFA
#define ID 0xD0
#define BME_SS PINB0

#define READ(x) ((1 << 7) | x)
#define WRITE(x) (~(1 << 7) & x)

void bme280_init(volatile uint8_t*, volatile uint8_t*, uint8_t);
uint8_t bme280_id();
uint16_t bme280_humidity();
uint32_t bme280_temeperature();
uint32_t bme280_pressure();

#endif