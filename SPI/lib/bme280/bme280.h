#ifndef BME280_H
#define BME280_H

#include "spi.h"

#define HUMIDITY 0xFD
#define PRESSURE 0xF7
#define TEMPERATURE 0xFA
#define ID 0xD0
#define BME_SS PINB0

#define SWAP(num) ((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000)

#define READ(x) ((1 << 7) | x)
#define WRITE(x) (~(1 << 7) & x)

spi_slave bme280_init(volatile uint8_t*, volatile uint8_t*, uint8_t);
uint32_t bme280_temeperature(spi_slave);

#endif