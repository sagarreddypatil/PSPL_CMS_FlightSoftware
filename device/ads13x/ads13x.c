#include "ads13x.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <spi.h>

// Commands
#define RESET 0b11001
#define STANDBY 0b100010
#define WAKEUP 0b0000000000110011
#define LOCK 0b0000010101010101
#define UNLOCK 0b0000011001010101
#define RREG 0b1010000000000000
#define WREG 0b0110000000000000

// Command Responses
#define RESET_RESP 0b1111111100100010
#define STANDBY_RESP 0b0000000000100010
#define WAKEUP_RESP 0b0000000000110011
#define LOCK_RESP 0b0000010101010101
#define UNLOCK_RESP 0b0000011001010101
#define RREG_RESP 0b1110000000000000
#define WREG_RESP 0b0100000000000000

#define REG_ADDR_MASK (1 << 6) - 1
#define REG_ADDR_SHIFT 7

#define REG_NUM_MASK (1 << 7) - 1
#define REG_NUM_SHIFT 0

// Mask and shift macro
#define MS(val, mask, shift) (((val) & (mask)) << (shift))  // for multiple bits
#define S(val, shift) ((val & 1) << (shift))                // for single bit

// num - 1 because 0 is 1 reg, don't pass 0, chip designers are dumb
#define REG_OP(op, reg, num)                     \
  (op | MS(reg, REG_ADDR_MASK, REG_ADDR_SHIFT) | \
   MS((num - 1), REG_NUM_MASK, REG_NUM_SHIFT))

#define REG_OP_SINGLE(op, reg) (op | MS(reg, REG_ADDR_MASK, REG_ADDR_SHIFT))

// Host byte-order to peripheral, 16, 24, 32 bit
// Remember to pad LSBs for data types shorter than the set word size
#define HTOP16(val) ((val >> 8) & 0xFF), (val & 0xFF)
#define HTOP24(val) ((val >> 16) & 0xFF), ((val >> 8) & 0xFF), (val & 0xFF)
#define HTOP32(val) \
  ((val >> 24) & 0xFF), ((val >> 16) & 0xFF), ((val >> 8) & 0xFF), (val & 0xFF)

// Peripheral byte-order to Host, 16, 24, 32 bit
#define PTOH16(ptr) ((ptr)[0] << 8 | (ptr)[1]) // short, swapping bytes
#define PTOH24(ptr) (((ptr)[0] << 24 | (ptr)[1] << 16 | (ptr)[2] << 8) >> 8) // sign extended 32-bit
#define PTOH32(ptr) ((ptr)[0] << 24 | (ptr)[1] << 16 | (ptr)[2] << 8 | (ptr)[3]) // just byte order swapping

void ads13x_reset(spi_device_t *device) {
  // try both 24-bit and 32-bit word size

  // uint8_t src24[4 * 4] = {HTOP16(RESET)};
  // uint8_t dst24[4 * 4];

  // SPI_TRANSFER(src24, dst24, 4 * 4);

  uint8_t src32[4 * 4] = {HTOP32(RESET)};
  uint8_t dst32[4 * 4] = {0};

  spi_write_read(device, src32, dst32, 4 * 4);
}

bool ads13x_ready(spi_device_t *device) {
  // ready waits for chip to start after a reset

  uint8_t src[4 * 3] = {0}; // default 3-byte word size
  uint8_t dst[4 * 3];

  spi_write_read(device, src, dst, 4 * 3);

  uint16_t status = PTOH16(dst);

  return (status & 0xFF00) == 0x0500;  // check status is init state
}

void ads13x_init(spi_device_t *device) {
  // set the chip to 32-bit word size
  uint16_t cmd = REG_OP_SINGLE(WREG, ads13x_mode);

  // also inadvertently disables SPI timeout, oh well, we don't need it
  uint16_t val = MS(0b11, 0b11, 8);  // set WLENGTH to 0b11
  val |= MS(0, 1, 10);               // clear RESET bit
  // ob11 is 32-bit word size with sign extension

  uint8_t src[4 * 4] = {HTOP16(cmd), 0, HTOP16(val), 0};
  uint8_t dst[4 * 4] = {0};

  spi_write_read(device, src, dst, 4 * 4);

  uint16_t resp     = (dst[0] << 8) | dst[1];
  uint16_t expected = REG_OP_SINGLE(WREG_RESP, ads13x_mode);

  // TOOD: error checking
}

void ads13x_wreg_single(spi_device_t *device, ads13x_reg_t reg, uint16_t data) {
  uint8_t src[4 * 4] = {
      HTOP16(REG_OP_SINGLE(WREG, reg)), 0, 0, HTOP16(data), 0, 0};
  uint8_t dst[4 * 4];

  spi_write_read(device, src, dst, 4 * 4);

  uint16_t resp     = (dst[0] << 8) | dst[1];
  uint16_t expected = REG_OP_SINGLE(WREG_RESP, reg);

  assert(resp == expected);
}

uint16_t ads13x_rreg_single(spi_device_t *device, ads13x_reg_t reg) {
  uint8_t src[4 * 4] = {HTOP16(REG_OP_SINGLE(RREG, reg))};
  uint8_t dst[4 * 4] = {0};

  spi_write_read(device, src, dst, 4 * 4);

  uint16_t resp = (dst[0] << 8) | dst[1];
  return resp;
}

void ads13x_set_sample_rate(spi_device_t *device, ads13x_sample_rate sample_rate) {
  uint16_t clock_reg = ads13x_rreg_single(device, ads13x_clock);
  clock_reg |= MS(sample_rate, 0b111, 2);

  ads13x_wreg_single(device, ads13x_clock, clock_reg);
}

bool ads13x_read_data(spi_device_t *device, uint16_t *status, int32_t *data,
                      uint32_t len) {
  if (len > 2) len = 2;

  uint8_t dst[4 * 4];
  spi_write_read(device, NULL, dst, 4 * 4);
  // for (int i = 0; i < 4 * 4; i++) {
  //   printf("%02X ", dst[i]);
  // }

  // printf("\n");

  *status = PTOH16(dst);
  for (int i = 0; i < len; i++) {
    data[i] = PTOH32(&dst[4 + (i * 4)]);
  }

  return true;
}

// void ads13x_set_gain(SPI_DEVICE_PARAM, ads13x_gain_setting gain_setting) {
//   ads13x_wreg_single(spi, ads13x_mode, GAIN(gain_setting, 0, 0, 0));
// }

// void ads13x_set_mode(SPI_DEVICE_PARAM, bool ch0, bool ch1, bool ch2, bool
// ch3,
//                      ads13x_sample_rate sample_rate,
//                      ads13x_pwr_setting pwr_setting) {
//   ads13x_wreg_single(spi, ads13x_mode,
//                      CLOCK(ch0, ch1, ch2, ch3, 0, sample_rate, pwr_setting));
// }
