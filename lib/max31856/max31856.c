#include <max31856.h>
#include <spi_device_impl.h>
#include <string.h>

SPI_MODE1;  // Mode 1 or 3 allowed, we're using 1

const uint baudrate = 5 * 1000 * 1000;
SPI_INITFUNC_IMPL(max31856, baudrate)

// MSB is 0 for read, 1 for write
#define READ(x) (x & 0x7F)
#define WRITE(x) (x | 0x80)

void max31856_rreg(SPI_PORT, uint8_t reg, void *data, size_t len) {
  uint8_t src[len + 1];
  uint8_t dst[len + 1];

  src[0] = READ(reg);  // first byte is the register address, with MSB 0 for read
  SPI_TRANSFER(src, dst, len + 1);

  memcpy(data, dst + 1, len);
}

void max31856_wreg(SPI_PORT, uint8_t reg, void *data, size_t len) {
  uint8_t src[len + 1];

  src[0] = WRITE(reg);  // first byte is the register address, with MSB 1 for write
  memcpy(src + 1, data, len);

  SPI_WRITE(src, len + 1);
}

void max31856_get_temp(SPI_PORT) {
}

void max31856_get_junction_temp(SPI_PORT) {
}