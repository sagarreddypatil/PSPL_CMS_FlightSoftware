#pragma once

#include <spi_device.h>

// Register Addresses
const uint8_t max31856_cr0    = 0x00;  // Configuration 0 Register
const uint8_t max31856_cr1    = 0x01;  // Configuration 1 Register
const uint8_t max31856_mask   = 0x02;  // Fault Mask Register
const uint8_t max31856_cjhf   = 0x03;  // Cold-Junction High Fault Threshold
const uint8_t max31856_cjlf   = 0x04;  // Cold-Junction Low Fault Threshold
const uint8_t max31856_lthfth = 0x05;  // Linearized Temperature High Fault Threshold MSB
const uint8_t max31856_lthftl = 0x06;  // Linearized Temperature High Fault Threshold LSB
const uint8_t max31856_ltlfth = 0x07;  // Linearized Temperature Low Fault Threshold MSB
const uint8_t max31856_ltlftl = 0x08;  // Linearized Temperature Low Fault Threshold LSB
const uint8_t max31856_cjto   = 0x09;  // Cold-Junction Temperature Offset Register
const uint8_t max31856_cjth   = 0x0A;  // Cold-Junction Temperature Register, MSB
const uint8_t max31856_cjtl   = 0x0B;  // Cold-Junction Temperature Register, LSB
const uint8_t max31856_ltcbh  = 0x0C;  // Linearized TC Temperature, Byte 2
const uint8_t max31856_ltcbm  = 0x0D;  // Linearized TC Temperature, Byte 1
const uint8_t max31856_ltcbl  = 0x0E;  // Linearized TC Temperature, Byte 0
const uint8_t max31856_sr     = 0x0F;  // Fault Status Register

// Register Default Values
const uint8_t max31856_cr0_default = 0x00;
const uint8_t max31856_cr1_default = 0x03;

// Register Bitmasks

// CR0 Bitmask
const uint8_t max31856_cr0_cmode    = 1 << 7;
const uint8_t max31856_cr0_1shot    = 1 << 6;
const uint8_t max31856_cr0_ocfault1 = 1 << 5;
const uint8_t max31856_cr0_ocfault0 = 1 << 4;
const uint8_t max31856_cr0_cj       = 1 << 3;
const uint8_t max31856_cr0_fault    = 1 << 2;
const uint8_t max31856_cr0_faultclr = 1 << 1;
const uint8_t max31856_cr0_5060hz   = 1 << 0;

// CR1 Bitmasks
const uint8_t max31856_cr1_reserved = 1 << 7;
const uint8_t max31856_cr1_avgsel2  = 1 << 6;
const uint8_t max31856_cr1_avgsel1  = 1 << 5;
const uint8_t max31856_cr1_avgsel0  = 1 << 4;
const uint8_t max31856_cr1_tc_type3 = 1 << 3;
const uint8_t max31856_cr1_tc_type2 = 1 << 2;
const uint8_t max31856_cr1_tc_type1 = 1 << 1;
const uint8_t max31856_cr1_tc_type0 = 1 << 0;

// MASK Bitmasks
const uint8_t max31856_mask_reserved  = 1 << 7;
const uint8_t max31856_mask_reserved2 = 1 << 6;
const uint8_t max31856_mask_cjhf      = 1 << 5;
const uint8_t max31856_mask_cjlf      = 1 << 4;
const uint8_t max31856_mask_tchf      = 1 << 3;
const uint8_t max31856_mask_tclf      = 1 << 2;
const uint8_t max31856_mask_ovuv      = 1 << 1;
const uint8_t max31856_mask_open      = 1 << 0;

// FR Bitmasks
const uint8_t max31856_fr_cj_range = 1 << 7;
const uint8_t max31856_fr_tc_range = 1 << 6;
const uint8_t max31856_fr_cjhigh   = 1 << 5;
const uint8_t max31856_fr_cjlow    = 1 << 4;
const uint8_t max31856_fr_tchigh   = 1 << 3;
const uint8_t max31856_fr_tclow    = 1 << 2;
const uint8_t max31856_fr_ovuv     = 1 << 1;
const uint8_t max31856_fr_open     = 1 << 0;

SPI_INITFUNC(max31856)

void max31856_rreg(SPI_DEVICE_PARAM, uint8_t reg, void *data, size_t len);
void max31856_wreg(SPI_DEVICE_PARAM, uint8_t reg, void *data, size_t len);
int32_t max31856_get_temp(SPI_DEVICE_PARAM);
int16_t max31856_get_junction_temp(SPI_DEVICE_PARAM);