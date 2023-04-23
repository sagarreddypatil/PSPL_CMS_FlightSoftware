#include "ads13x.h"

#include <spi_device_impl.h>
#include <stdio.h>

SPI_MODE1;  // Mode 1 or 3 allowed, we're using 1

static const uint baudrate = 100000;
SPI_INITFUNC_IMPL(ads13x, baudrate)

#define WREG_AND_MASK 0x7FFF  //  0b0111111111111111
#define WREG_OR_MASK 0x6000   //  0b0110000000000000

#define RREG_AND_MASK 0xbFFF  //  0b1011111111111111
#define RREG_OR_MASK 0xa000   //  0b1010000000000000

//                                 FEDCBA9876543210
#define MODE_AND_MASK 0x3F12  // 0b0011111100010010
#define GAIN_AND_MASK 0x7777  // 0b0111011101110111
#define CLOCK_AND_MASK 0xF9F  // 0b0000111110011111
#define CH_CFG_AND_MASK 0x3   // 0b0000000000000011

// Mask and shift macro
#define MS(val, mask, shift) (((val) & (mask)) << (shift))  // for multiple bits
#define S(val, shift) ((val & 1) << (shift))                // for single bit

#define WREG(a, n)                                                   \
  MASK((SET_BITS(a, 7, 6) | SET_BITS((n - 1), 0, 6)), WREG_AND_MASK, \
       WREG_OR_MASK)

#define RREG(a, n)                                                   \
  MASK((SET_BITS(a, 0, 6) | SET_BITS((n - 1), 0, 6)), RREG_AND_MASK, \
       RREG_OR_MASK)

#define MODE(REG_CRC_EN, RX_CRC_EN, CRC_TYPE, RESET, WLENGTH, TIMEOUT, \
             DRDY_HiZ)                                                 \
  MASK(SET_BIT(REG_CRC_EN, 13) | SET_BIT(RX_CRC_EN, 12) |              \
           SET_BIT(CRC_TYPE, 11) | SET_BIT(RESET, 10) |                \
           SET_BITS(WLENGTH, 8, 2) | SET_BIT(TIMEOUT, 4) |             \
           SET_BIT(DRDY_HiZ, 1),                                       \
       MODE_AND_MASK, 0)

// Gain
#define GAIN(CH0, CH1, CH2, CH3)                                          \
  MASK(SET_BITS(CH0, 12, 3) | SET_BITS(CH1, 8, 3) | SET_BITS(CH2, 4, 3) | \
           SET_BITS(CH3, 0, 3),                                           \
       GAIN_AND_MASK, 0)

// Clock

#define CLOCK(CH0_EN, CH1_EN, CH2_EN, CH3_EN, CLK_SEL, OSR, PWR)            \
  MASK(SET_BIT(CH3_EN, 11) | SET_BIT(CH2_EN, 10) | SET_BIT(CH1_EN, 9) |     \
           SET_BIT(CH0_EN, 8) | SET_BIT(CLK_SEL, 7) | SET_BITS(OSR, 2, 2) | \
           SET_BITS(PWR, 0, 2),                                             \
       CLOCK_AND_MASK, 0)

// Channel Configuration

enum CH_CFG_SETTING {
  AIN0P_AND_AIN0N             = 0,  // AIN0P and AIN0N
  AIN0_DISCONNECTED_ADC_SHORT = 1,  // AIN0 disconnected, ADC inputs shorted
  POS_DC_TEST_SIGNAL          = 2,  // Positive DC test signal
  NEG_DC_TEST_SIGNAL          = 3   // Negative DC test signal
};

#define CH_CFG(MUX) MASK(SET_BITS(MUX, 0, 2), CH_CFG_AND_MASK, 0)

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                \
  (byte & 0x80 ? '1' : '0'), (byte & 0x40 ? '1' : '0'),     \
      (byte & 0x20 ? '1' : '0'), (byte & 0x10 ? '1' : '0'), \
      (byte & 0x08 ? '1' : '0'), (byte & 0x04 ? '1' : '0'), \
      (byte & 0x02 ? '1' : '0'), (byte & 0x01 ? '1' : '0')

void ads13x_config(SPI_DEVICE_PARAM) {
}

void ads13x_wreg_single(SPI_DEVICE_PARAM, ads13x_reg_t reg, uint16_t data) {
}

uint16_t ads13x_rreg_single(SPI_DEVICE_PARAM, ads13x_reg_t reg) {
  uint8_t rreg_cmd = RREG(reg, 1);
  uint8_t cmd[3]   = {rreg_cmd, 0, 0};
  uint8_t data[12] = {};
  SPI_TRANSFER(cmd, data, 3);
  SPI_READ(data + 3, 9);
  for (int i = 0; i < 12; i++) {
    printf("%02x ", data[i]);
  }
  printf("\n");
  //  uint16_t register_data = (data[0] << 8) | data[1];
  return 0;
}
#define data_len 12
// int global_counter = 1;
uint32_t ads13x_read_data(SPI_DEVICE_PARAM) {
  uint8_t data[data_len] = {};
  for (int i = 0; i < 1; i++) {
    SPI_READ(data, data_len);
  }
  //  global_counter++;
  //  if (global_counter % 100 == 0) {
  for (int i = 0; i < data_len; i++) {
    printf("0x%02X ", data[i]);
  }
  printf("\n");

  return 0;
}

void ads13x_set_gain(SPI_DEVICE_PARAM, ads13x_gain_setting gain_setting) {
  ads13x_wreg_single(spi, ads13x_mode, GAIN(gain_setting, 0, 0, 0));
}

void ads13x_set_mode(SPI_DEVICE_PARAM, bool ch0, bool ch1, bool ch2, bool ch3,
                     ads13x_sample_rate sample_rate,
                     ads13x_pwr_setting pwr_setting) {
  ads13x_wreg_single(spi, ads13x_mode,
                     CLOCK(ch0, ch1, ch2, ch3, 0, sample_rate, pwr_setting));
}
