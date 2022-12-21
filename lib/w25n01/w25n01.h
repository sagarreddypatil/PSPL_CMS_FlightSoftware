#pragma once

#include <spi_device.h>

/*
 * Instruction Definitions
 */
static const uint8_t w25n01_ins_reset                    = 0xFF;
static const uint8_t w25n01_ins_jedec_id                 = 0x9F;
static const uint8_t w25n01_ins_read_status_register     = 0x0F;
static const uint8_t w25n01_ins_write_status_register    = 0x1F;
static const uint8_t w25n01_ins_write_enable             = 0x06;
static const uint8_t w25n01_ins_write_disable            = 0x04;
static const uint8_t w25n01_ins_bbm_swap                 = 0xA1;
static const uint8_t w25n01_ins_read_bbm_lut             = 0xA5;
static const uint8_t w25n01_ins_ecc_fail_addr            = 0xA9;
static const uint8_t w25n01_ins_block_erase              = 0xD8;
static const uint8_t w25n01_ins_program_load             = 0x02;
static const uint8_t w25n01_ins_random_program_load      = 0x84;
static const uint8_t w25n01_ins_quad_program_load        = 0x32;
static const uint8_t w25n01_ins_random_quad_program_load = 0x34;
static const uint8_t w25n01_ins_program_execute          = 0x10;
static const uint8_t w25n01_ins_page_data_read           = 0x13;
static const uint8_t w25n01_ins_read                     = 0x03;
static const uint8_t w25n01_ins_fast_read                = 0x0B;
static const uint8_t w25n01_ins_fast_read_4byte_addr     = 0x0C;
// bunch of others i didn't feel like defining

/*
 * Status Register Address Definitions
 */
static const uint8_t w25n01_protection_register = 0xA0;
static const uint8_t w25n01_config_register     = 0xB0;
static const uint8_t w25n01_status_register     = 0xC0;

typedef struct {
  bool enabled;
  bool invalid;
  uint16_t logical_block_address;
  uint16_t physical_block_address;
} w25n01_bbm_entry;

/*
 * Function Signatures
 */
SPI_INITFUNC(w25n01);

void w25n01_transfer(SPI_DEVICE_PARAM, void* src, void* dst, size_t len);
void w25n01_write(SPI_DEVICE_PARAM, void* src, size_t len);

void w25n01_read_buffer(SPI_DEVICE_PARAM, uint16_t col, void* buf, size_t len);                // read from the current page in the buffer
void w25n01_write_buffer(SPI_DEVICE_PARAM, uint16_t col, void* buf, size_t len, bool random);  // write to the current page in the buffer, needs write enable

void w25n01_write_status_register(SPI_DEVICE_PARAM, uint8_t addr, uint8_t value);
uint8_t w25n01_read_status_register(SPI_DEVICE_PARAM, uint8_t addr);

void bbm_add(SPI_DEVICE_PARAM, uint16_t logical_block_address, uint16_t physical_block_address);  // note it's block address, not page address, needs write enable
void bbm_read(SPI_DEVICE_PARAM, w25n01_bbm_entry* entries);

static inline bool busy(SPI_DEVICE_PARAM) {
  return w25n01_read_status_register(spi, 0) & 0x01;
}

// This is used for instructions that only need the instruction byte
#define __INSTR_SINGLE(name, ins)                      \
  static inline void w25n01_##name(SPI_DEVICE_PARAM) { \
    uint8_t src[] = {ins};                             \
    w25n01_write(spi, src, 1);                         \
  }

__INSTR_SINGLE(reset, w25n01_ins_reset);
__INSTR_SINGLE(write_enable, w25n01_ins_write_enable);
__INSTR_SINGLE(write_disable, w25n01_ins_write_disable);

#undef __INSTR_SINGLE

#define __INSTR_PAGE(name, ins)                                       \
  static inline void w25n01_##name(SPI_DEVICE_PARAM, uint16_t addr) { \
    uint8_t src[4] = {ins, 0, addr >> 8, addr & 0xFF};                \
    SPI_WRITE(src, 4);                                                \
  }

__INSTR_PAGE(read_page, w25n01_ins_page_data_read);
__INSTR_PAGE(write_page, w25n01_ins_program_execute);  // needs write enable
__INSTR_PAGE(block_erase, w25n01_ins_block_erase);     // needs write enable

#undef __INSTR_PAGE