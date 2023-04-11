#pragma once

#include <spi_device.h>

// Register Addresses

typedef enum {
  ads13x_id              = 0x00,
  ads13x_status          = 0x01,
  ads13x_mode            = 0x02,
  ads13x_clock           = 0x03,
  ads13x_gain            = 0x04,
  ads13x_global_chop_cfg = 0x06,
  ads13x_ch0_cfg         = 0x09,
  ads13x_ch1_cfg         = 0x0E,
  ads13x_ch2_cfg         = 0x13,
  ads13x_ch3_cfg         = 0x18,
  ads13x_regmap_crc      = 0x3E,
} ads13x_reg_t;

typedef enum GAIN_SETTING {
  GAIN_1   = 0,
  GAIN_2   = 1,
  GAIN_4   = 2,
  GAIN_8   = 3,
  GAIN_16  = 4,
  GAIN_32  = 5,
  GAIN_64  = 6,
  GAIN_128 = 7,
} ads13x_GAIN_SETTING;

typedef enum PWR_SETTING {
  VERY_LOW_POWER  = 0,
  LOW_POWER       = 1,
  HIGH_RESOLUTION = 2,
} ads13x_PWR_SETTING;

typedef enum SAMPLE_RATE {
  SR_32K = 0,  // OSR = 128
  SR_16K = 1,
  SR_8K  = 2,
  SR_4K  = 3,
  SR_2K  = 4,
  SR_1K  = 5,
  SR_500 = 6,
  SR_250 = 7  // OSR = 16384
} ads13x_SAMPLE_RATE;

SPI_INITFUNC(ads13x);

uint16_t ads13x_rreg_single(SPI_DEVICE_PARAM, ads13x_reg_t reg);
void ads13x_wreg_single(SPI_DEVICE_PARAM, ads13x_reg_t reg, uint16_t data);
void ads13x_init(SPI_DEVICE_PARAM);

// Read Data
uint32_t ads13x_read_data(SPI_DEVICE_PARAM);

// Convenience Functions

void ads13x_set_gain(SPI_DEVICE_PARAM, ads13x_GAIN_SETTING gain_setting);

void ads13x_set_mode(SPI_DEVICE_PARAM, bool ch0, bool ch1, bool ch2, bool ch3, ads13x_SAMPLE_RATE sampleRate,
                     ads13x_PWR_SETTING powerSetting);