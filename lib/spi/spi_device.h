#pragma once

#include <hardware/spi.h>
#include <pico/stdlib.h>

#define SPI_INITFUNC(name) \
  uint name##_set(spi_inst_t *spi);

#define SPI_DEVICE_PARAM \
  spi_inst_t *spi

#define SPI_DEVICE \
  spi
