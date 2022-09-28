#pragma once

#include <hardware/spi.h>
#include <pico/stdlib.h>

typedef struct {
  spi_inst_t *spi_inst;
  uint cs;
} spi_device_t;

#define SPI_INITFUNC(name) \
  uint name##_set(spi_device_t *spi);

#define SPI_DEVICE_PARAM \
  spi_device_t *spi

#define SPI_INST \
  (spi->spi_inst)
