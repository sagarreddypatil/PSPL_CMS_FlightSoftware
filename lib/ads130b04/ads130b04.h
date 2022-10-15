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

SPI_INITFUNC(ads130b04);

uint16_t ads130b04_rreg_single(SPI_DEVICE_PARAM, uint8_t reg, size_t len);
void ads130b04_wreg_single(SPI_DEVICE_PARAM, uint8_t reg, uint16_t data, size_t len);
void ads130b04_init(SPI_DEVICE_PARAM);

// Read Data

uint16_t ads130b04_read_data(SPI_DEVICE_PARAM);