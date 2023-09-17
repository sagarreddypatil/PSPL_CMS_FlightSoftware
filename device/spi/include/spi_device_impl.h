#pragma once

#include <hardware/spi.h>
#include <pico/stdlib.h>
#include <spi_device.h>

/// @brief Initialize CS pin as output and set it high
/// @param cs_pin
static inline void cs_init(uint cs_pin) {
  gpio_init(cs_pin);
  gpio_put(cs_pin, 1);
  gpio_set_dir(cs_pin, GPIO_OUT);
}

/// @brief Set CS pin low to enable
/// @param cs_pin
static inline void cs_select(uint cs_pin) {
  asm volatile("nop \n nop \n nop");  // from pico examples, dunno why but
                                      // monkey see monkey do
  gpio_put(cs_pin, 0);
  asm volatile("nop \n nop \n nop");
}

/// @brief Set CS pin high to disable
/// @param cs_pin
static inline void cs_deselect(uint cs_pin) {
  asm volatile("nop \n nop \n nop");
  gpio_put(cs_pin, 1);
  asm volatile("nop \n nop \n nop");
}

#define SPI_MODE0                            \
  static const spi_cpol_t cpol = SPI_CPOL_0; \
  static const spi_cpha_t cpha = SPI_CPHA_0;

#define SPI_MODE1                            \
  static const spi_cpol_t cpol = SPI_CPOL_0; \
  static const spi_cpha_t cpha = SPI_CPHA_1;

#define SPI_MODE2                            \
  static const spi_cpol_t cpol = SPI_CPOL_1; \
  static const spi_cpha_t cpha = SPI_CPHA_0;

#define SPI_MODE3                            \
  static const spi_cpol_t cpol = SPI_CPOL_1; \
  static const spi_cpha_t cpha = SPI_CPHA_1;

#define SPI_INITFUNC_IMPL(name, baud)                       \
  uint name##_set(SPI_DEVICE_PARAM) {                       \
    cs_init(spi->cs);                                       \
    uint actual_baud = spi_set_baudrate(SPI_INST, baud);    \
    spi_set_format(SPI_INST, 8, cpol, cpha, SPI_MSB_FIRST); \
    return actual_baud;                                     \
  }

#define SPI_BOILERPLATE(x) \
  cs_select(spi->cs);      \
  x;                       \
  cs_deselect(spi->cs);

#define SPI_TRANSFER(src, dst, len) \
  SPI_BOILERPLATE(spi_write_read_blocking(SPI_INST, src, dst, len))

#define SPI_TRANSFER16(src, dst, len) \
  SPI_BOILERPLATE(spi_write16_read16_blocking(SPI_INST, src, dst, len))

#define SPI_WRITE(src, len) \
  SPI_BOILERPLATE(spi_write_blocking(SPI_INST, src, len))
#define SPI_WRITE16(src, len) \
  SPI_BOILERPLATE(spi_write16_blocking(SPI_INST, src, len))

#define SPI_READ(dst, len) \
  SPI_BOILERPLATE(spi_read_blocking(SPI_INST, 0, dst, len))
#define SPI_READ16(dst, len) \
  SPI_BOILERPLATE(spi_read16_blocking(SPI_INST, 0, dst, len))