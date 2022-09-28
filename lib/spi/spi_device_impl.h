#ifndef SPI_DEVICE_IMPL_H
#define SPI_DEVICE_IMPL_H

#include <hardware/spi.h>
#include <pico/stdlib.h>
#include <spi_device.h>

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

#define SPI_INITFUNC_IMPL(name, baud)                  \
  void name##_set(spi_inst_t *spi) {                   \
    spi_set_baudrate(spi, baud);                       \
    spi_set_format(spi, 8, cpol, cpha, SPI_MSB_FIRST); \
  }

#define SPI_TRANSFER(src, dst, len) spi_write_read_blocking(spi, src, dst, len)
#define SPI_TRANSFER16(src, dst, len) spi_write16_read16_blocking(spi, src, dst, len)

#define SPI_WRITE(src, len) spi_write_blocking(spi, src, len)
#define SPI_WRITE16(src, len) spi_write16_blocking(spi, src, len)

#define SPI_READ(dst, len) spi_read_blocking(spi, 0, dst, len)
#define SPI_READ16(dst, len) spi_read16_blocking(spi, 0, dst, len)

#endif  // SPI_DEVICE_IMPL_H