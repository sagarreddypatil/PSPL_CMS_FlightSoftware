#pragma once

#include <hardware/spi.h>
#include <pico/stdlib.h>

#define SPI_INITFUNC(name) \
  void name##_set(spi_inst_t *spi);

#define SPI_DEVICE_PARAM \
  spi_inst_t *spi

#define SPI_DEVICE \
  spi
