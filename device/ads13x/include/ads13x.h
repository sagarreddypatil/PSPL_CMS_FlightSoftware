#pragma once

#include <myspi.h>

// Register Addresses

typedef enum {
  ads13x_id         = 0x00,
  ads13x_status     = 0x01,
  ads13x_mode       = 0x02,
  ads13x_clock      = 0x03,
  ads13x_gain       = 0x04,
  ads13x_cfg        = 0x06,
  ads13x_ch0_cfg    = 0x09,
  ads13x_ch1_cfg    = 0x0E,
  ads13x_ch2_cfg    = 0x13,
  ads13x_regmap_crc = 0x3E,
} ads13x_reg_t;

typedef enum {
  GAIN_1   = 0,
  GAIN_2   = 1,
  GAIN_4   = 2,
  GAIN_8   = 3,
  GAIN_16  = 4,
  GAIN_32  = 5,
  GAIN_64  = 6,
  GAIN_128 = 7,
} ads13x_gain_setting;

typedef enum {
  VERY_LOW_POWER  = 0,
  LOW_POWER       = 1,
  HIGH_RESOLUTION = 2,
} ads13x_pwr_setting;

typedef enum {
  SR_32K = 0,  // OSR = 128
  SR_16K = 1,
  SR_8K  = 2,
  SR_4K  = 3,
  SR_2K  = 4,
  SR_1K  = 5,
  SR_500 = 6,
  SR_250 = 7  // OSR = 16384
} ads13x_sample_rate;

void ads13x_reset(spi_device_t *device);
bool ads13x_ready(spi_device_t *device);
void ads13x_init(spi_device_t *device);

uint16_t ads13x_rreg_single(spi_device_t *device, ads13x_reg_t reg);
void ads13x_wreg_single(spi_device_t *device, ads13x_reg_t reg, uint16_t data);

void ads13x_set_sample_rate(spi_device_t *device, ads13x_sample_rate sample_rate);

// Read Data
// check that len is <= adc channels
bool ads13x_read_data(spi_device_t *device, uint16_t *status, int32_t *data,
                      uint32_t len);

// Convenience Functions
void ads13x_set_gain(spi_device_t *device, ads13x_gain_setting gain_setting);

void ads13x_set_mode(spi_device_t *device, bool ch0, bool ch1, bool ch2, bool ch3,
                     ads13x_sample_rate sample_rate,
                     ads13x_pwr_setting pwr_setting);