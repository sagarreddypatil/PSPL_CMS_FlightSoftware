#include <max31856.h>
#include <spi_device_impl.h>
#include <string.h>

/*
 * README!!!
 *
 * Quick summary of how this chip works
 * Basically there are 16 registers addressed from 0x00 to 0x0F
 * The register address definitions are in the header file
 * The registers are each 1 byte.
 *
 * In order to read or write to a register, the first byte in the SPI transmission needs to be the register address
 * If the most significant bit of the register address is 0, then the chip will read the register
 * If it's 1, then the chip will write to the register
 *
 * If you command the chip to read, it'll ignore any subsequent bytes in the transmission, and will respond with the register value
 * If you keep cycling the clock after one register value is sent, it'll send the values stored in subsequent registers, for however many bytes you read
 * The end of a transmission is marked by driving CS(chip select) high.
 *
 * Writing follows the same pattern, except the chip doesn't respond with anything. You just write a byte or multiple bytes, and then drive CS high.
 */

SPI_MODE3;  // Mode 1 or 3 allowed, we're using 3

const uint baudrate = 5 * 1000 * 1000;  // 5 MHz, max for MAX31856
SPI_INITFUNC_IMPL(max31856, baudrate)

// MSB is 0 for read, 1 for write
#define READ(x) (x & 0x7F)
#define WRITE(x) (x | 0x80)

uint8_t max31856_rreg_byte(SPI_DEVICE_PARAM, uint8_t reg) {
  uint8_t src[2] = {READ(reg)};
  uint8_t dst[2];
  SPI_TRANSFER(src, dst, 2);
  return dst[1];
}

void max31856_wreg_byte(SPI_DEVICE_PARAM, uint8_t reg, uint8_t data) {
  uint8_t src[2] = {WRITE(reg), data};
  SPI_WRITE(src, 2);
}

void max31856_rreg(SPI_DEVICE_PARAM, uint8_t reg, void *data, size_t len) {
  uint8_t src[len + 1];
  uint8_t dst[len + 1];

  src[0] = READ(reg);  // first byte is the register address, with MSB 0 for read
  SPI_TRANSFER(src, dst, len + 1);

  memcpy(data, dst + 1, len);
}

void max31856_wreg(SPI_DEVICE_PARAM, uint8_t reg, void *data, size_t len) {
  uint8_t src[len + 1];

  src[0] = WRITE(reg);  // first byte is the register address, with MSB 1 for write
  memcpy(src + 1, data, len);

  SPI_WRITE(src, len + 1);
}

int32_t max31856_get_tc_temp(SPI_DEVICE_PARAM) {
  uint8_t data[3];
  max31856_rreg(spi, max31856_ltcbh, data, 3);  // read ltcbh, ltcbm, ltctl into buffer

  /*
   * The integers we read are two's complement signed, so we place read bits in
   * the most significant bits of the integer, and then shift the integer right
   * to automatically sign extend.
   */
  int32_t temp = (data[0] << 24) | (data[1] << 16) | (data[2] << 8);

  // Only 19 bits of the 24 we read are used. So only first 19 bits of the 32-bit int are used
  temp >>= (32 - 19);

  return temp;
}

int16_t max31856_get_cj_temp(SPI_DEVICE_PARAM) {
  // same process as before, but less shenanigans because this is just a usual 16-bit signed integer
  uint8_t data[2];
  max31856_rreg(spi, max31856_cjth, data, 2);

  int16_t temp = (data[0] << 8) | data[1];  // to deal with endianness

  return temp;
}