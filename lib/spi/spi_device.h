#pragma once

#include <hardware/spi.h>
#include <pico/stdlib.h>

#define SPI_INITFUNC(name) \
  void name##_init(spi_inst_t *spi);

#define SPI_PORT \
  spi_inst_t *spi
