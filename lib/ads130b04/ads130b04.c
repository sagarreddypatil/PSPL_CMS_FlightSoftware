#include "ads130b04.h"

#include <spi_device_impl.h>
#include <string.h>

SPI_MODE3;  // Mode 1 or 3 allowed, we're using 1

const uint baudrate = 5 * 1000 * 1000;  // 5 MHz, max for MAX31856
SPI_INITFUNC_IMPL(ads130b04, baudrate)

#define WREG_AND_MASK 0x7FFF  //  0b0111111111111111
#define WREG_OR_MASK 0x6000   //  0b0110000000000000

#define RREG_AND_MASK 0xbfff  //  0b1011111111111111
#define RREG_OR_MASK 0xa000   //  0b1010000000000000

//                                 FEDCBA9876543210
#define MODE_AND_MASK 0x3f12  // 0b0011111100010010

#define GAIN_AND_MASK 0x7777  // 0b0111011101110111

#define CH_CFG_AND_MASK 0x3  // 0b0000000000000011

#define MASK(x, and, or) (((x) & and) | or)

#define SET_BITS(x, pos, len) ((x << pos) & ((1 << (len + pos + 1)) - 1))
#define SET_BIT(x, pos) SET_BITS(x, pos, 1)

#define WREG(a, n) MASK((SET_BITS(a, 0, 6) | SET_BITS(n, 0, 6)), WREG_AND_MASK, WREG_OR_MASK)
#define READ(a, n) MASK((SET_BITS(a, 0, 6) | SET_BITS(n, 0, 6)), RREG_AND_MASK, RREG_OR_MASK)

#define MODE(REG_CRC_EN, RX_CRC_EN, CRC_TYPE, RESET, WLENGTH, TIMEOUT, DRDY_HiZ)  \
  MASK(SET_BIT(REG_CRC_EN, 13) | SET_BIT(RX_CRC_EN, 12) | SET_BIT(CRC_TYPE, 11) | \
           SET_BIT(RESET, 10) | SET_BITS(WLENGTH, 8, 2) | SET_BIT(TIMEOUT, 4) |   \
           SET_BIT(DRDY_HiZ, 1),                                                  \
       MODE_AND_MASK, 0)

enum GAIN_SETTINGS {
  GAIN_1   = 0,
  GAIN_2   = 1,
  GAIN_4   = 2,
  GAIN_8   = 3,
  GAIN_16  = 4,
  GAIN_32  = 5,
  GAIN_64  = 6,
  GAIN_128 = 7,
};

#define GAIN(CH0, CH1, CH2, CH3) \
  MASK(SET_BITS(CH0, 12, 3) | SET_BITS(CH1, 8, 3) | SET_BITS(CH2, 4, 3) | SET_BITS(CH3, 0, 3), GAIN_AND_MASK, 0)

enum CH_CFG_SETTINGS {
  AIN0P_AND_AIN0N             = 0,  // AIN0P and AIN0N
  AIN0_DISCONNECTED_ADC_SHORT = 1,  // AIN0 disconnected, ADC inputs shorted
  POS_DC_TEST_SIGNAL          = 2,  // Positive DC test signal
  NEG_DC_TEST_SIGNAL          = 3   // Negative DC test signal
};

#define CH_CFG(MUX) \
  MASK(SET_BITS(MUX, 0, 2), CH_CFG_AND_MASK, 0)

void ads130b04_init(SPI_DEVICE_PARAM) {
  uint16_t command = WREG(ads130b04_mode, 1);

  uint16_t data  = MODE(0, 0, 1, 1, 0, 1, 0);
  uint8_t src[6] = {0, command >> 8, command & 0xFF, 0, data >> 8, data & 0xFF};
  SPI_WRITE(src, 6);
}

void ads130b04_wreg_single(SPI_DEVICE_PARAM, uint8_t reg, uint16_t data, size_t len) {
  uint16_t command[2] = {WREG(reg, 1), data};
  SPI_WRITE16(command, 2);
}

uint16_t ads130b04_rreg_single(SPI_DEVICE_PARAM, uint8_t reg, size_t len) {
  uint16_t data[6];

  uint16_t command = READ(reg, len);
  SPI_WRITE16(&command, 1);
  SPI_READ16(data, len);
  return data[5]; // TODO: Get data
}


uint16_t ads130b04_read_data(SPI_DEVICE_PARAM) {
    uint16_t data[6];
    SPI_READ16(data,6);
    return data[0];
}