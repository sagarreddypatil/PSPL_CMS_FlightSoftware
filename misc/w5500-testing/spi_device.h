#pragma once

#include <hardware/spi.h>
#include <pico/stdlib.h>

/// @brief Struct to hold SPI device parameters, i.e., SPI hardware pointer and CS pin
typedef struct {
  spi_inst_t *spi_inst;
  uint cs;
} spi_device_t;

#define SPI_DEVICE(name, spi_port, cs_pin) \
  static const spi_device_t _##name = {    \
      .spi_inst = spi_port,                \
      .cs       = cs_pin,                  \
  };                                       \
  static const spi_device_t *name = &_##name;

#define SPI_DEVICE_PARAM \
  const spi_device_t *spi

#define SPI_INITFUNC(name) \
  uint name##_set(SPI_DEVICE_PARAM);

#define SPI_INST \
  (spi->spi_inst)




