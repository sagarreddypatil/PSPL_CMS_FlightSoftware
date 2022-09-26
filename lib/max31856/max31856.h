#pragma once

#include <spi_device.h>

// Generated in Google Sheets from the datasheet
const uint8_t max31856_cr0 = 0x00;     // Configuration 0 Register
const uint8_t max31856_cr1 = 0x01;     // Configuration 1 Register
const uint8_t max31856_mask = 0x02;    // Fault Mask Register
const uint8_t max31856_cjhf = 0x03;    // Cold-Junction High Fault Threshold
const uint8_t max31856_cjlf = 0x04;    // Cold-Junction Low Fault Threshold
const uint8_t max31856_lthfth = 0x05;  // Linearized Temperature High Fault Threshold MSB
const uint8_t max31856_lthftl = 0x06;  // Linearized Temperature High Fault Threshold LSB
const uint8_t max31856_ltlfth = 0x07;  // Linearized Temperature Low Fault Threshold MSB
const uint8_t max31856_ltlftl = 0x08;  // Linearized Temperature Low Fault Threshold LSB
const uint8_t max31856_cjto = 0x09;    // Cold-Junction Temperature Offset Register
const uint8_t max31856_cjth = 0x0A;    // Cold-Junction Temperature Register, MSB
const uint8_t max31856_cjtl = 0x0B;    // Cold-Junction Temperature Register, LSB
const uint8_t max31856_ltcbh = 0x0C;   // Linearized TC Temperature, Byte 2
const uint8_t max31856_ltcbm = 0x0D;   // Linearized TC Temperature, Byte 1
const uint8_t max31856_ltcbl = 0x0E;   // Linearized TC Temperature, Byte 0
const uint8_t max31856_sr = 0x0F;      // Fault Status Register

SPI_INITFUNC(max31856)

inline void max31856_rreg(SPI_DEVICE_PARAM, uint8_t reg, void *data, size_t len);
inline void max31856_wreg(SPI_DEVICE_PARAM, uint8_t reg, void *data, size_t len);
inline int32_t max31856_get_temp(SPI_DEVICE_PARAM);
inline int16_t max31856_get_junction_temp(SPI_DEVICE_PARAM);