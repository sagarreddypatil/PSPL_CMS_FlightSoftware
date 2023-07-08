#pragma once

#include <hardware/spi.h>
#include <hardware/gpio.h>
#include <pico/stdlib.h>

/// @brief Struct to hold SPI device parameters, i.e., SPI hardware pointer and
/// CS pin
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

#define SPI_DEVICE_PARAM const spi_device_t *spi

#define SPI_INITFUNC(name) uint name##_set(SPI_DEVICE_PARAM);

#define SPI_INST (spi->spi_inst)

static inline void spi_init_bus(spi_inst_t *spi, uint poci, uint pico,
                                uint sclk) {
  spi_init(spi, 0);

  gpio_set_function(poci, GPIO_FUNC_SPI);
  gpio_set_function(pico, GPIO_FUNC_SPI);
  gpio_set_function(sclk, GPIO_FUNC_SPI);
}

static inline void spi_init_bus_adv(spi_inst_t *spi, uint poci, uint pico,
                                    uint sclk, enum gpio_slew_rate slew,
                                    enum gpio_drive_strength drive) {
  spi_init_bus(spi, poci, pico, sclk);

  gpio_set_slew_rate(poci, slew);
  gpio_set_slew_rate(pico, slew);
  gpio_set_slew_rate(sclk, slew);

  gpio_set_drive_strength(poci, drive);
  gpio_set_drive_strength(pico, drive);
  gpio_set_drive_strength(sclk, drive);
}
