#pragma once

#include <spi_device.h>

// Register Addresses

static const uint8_t ads130b04_id              = 0x0;
static const uint8_t ads130b04_status          = 0x01;
static const uint8_t ads130b04_mode            = 0x02;
static const uint8_t ads130b04_clock           = 0x03;
static const uint8_t ads130b04_gain            = 0x04;
static const uint8_t ads130b04_global_chop_cfg = 0x06;
static const uint8_t ads130b04_ch0_cfg         = 0x09;
static const uint8_t ads130b04_ch1_cfg         = 0x0E;
static const uint8_t ads130b04_ch2_cfg         = 0x13;
static const uint8_t ads130b04_ch3_cfg         = 0x18;
static const uint8_t ads130b04_regmap_crc      = 0x3E;

typedef enum GAIN_SETTING {
  GAIN_1   = 0,
  GAIN_2   = 1,
  GAIN_4   = 2,
  GAIN_8   = 3,
  GAIN_16  = 4,
  GAIN_32  = 5,
  GAIN_64  = 6,
  GAIN_128 = 7,
} ads130b04_GAIN_SETTING;

typedef enum PWR_SETTING {
  VERY_LOW_POWER  = 0,
  LOW_POWER       = 1,
  HIGH_RESOLUTION = 2,
} ads130b04_PWR_SETTING;

typedef enum SAMPLE_RATE {
  SR_32K = 0,  // OSR = 128
  SR_16K = 1,
  SR_8K  = 2,
  SR_4K  = 3,
  SR_2K  = 4,
  SR_1K  = 5,
  SR_500 = 6,
  SR_250 = 7  // OSR = 16384
} ads130b04_SAMPLE_RATE;

SPI_INITFUNC(ads130b04);

uint16_t ads130b04_rreg_single(SPI_DEVICE_PARAM, uint8_t reg);
void ads130b04_wreg_single(SPI_DEVICE_PARAM, uint8_t reg, uint16_t data);
void ads130b04_init(SPI_DEVICE_PARAM);

// Read Data
uint16_t ads130b04_read_data(SPI_DEVICE_PARAM);

// Convenience Functions

void ads130b04_set_gain(SPI_DEVICE_PARAM, ads130b04_GAIN_SETTING gain_setting);

void ads130b04_set_sample_rate(SPI_DEVICE_PARAM, bool ch0, bool ch1, bool ch2, bool ch3, ads130b04_SAMPLE_RATE sampleRate);