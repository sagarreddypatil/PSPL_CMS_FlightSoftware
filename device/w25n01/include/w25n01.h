#pragma once

#include <myspi.h>

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
typedef enum {
    w25n01_protection_register = 0xA0,
    w25n01_config_register     = 0xB0,
    w25n01_status_register     = 0xC0
} w25n01_status_register_t;

typedef struct {
    bool enabled;
    bool invalid;
    uint16_t logical_block_address;
    uint16_t physical_block_address;
} w25n01_bbm_entry;

typedef struct {
    uint8_t mfrId;
    uint16_t deviceId;
} w25n01_jedec_id_t;

/*
 * Function Signatures
 */

void w25n01_transfer(SPI_DEVICE_PARAM, void* src, void* dst,
                     size_t len);  // basic transfer function for all of library
void w25n01_write(SPI_DEVICE_PARAM, void* src,
                  size_t len);  // write only, without read

w25n01_jedec_id_t w25n01_read_jedec_id(SPI_DEVICE_PARAM);  // read the jedec id
                                                           // of the chip

void w25n01_read_buffer(
    SPI_DEVICE_PARAM, uint16_t col, void* buf,
    size_t len);  // read from the current page in the buffer
void w25n01_write_buffer(SPI_DEVICE_PARAM, uint16_t col, void* buf, size_t len,
                         bool random);  // write to the current page in the
                                        // buffer, needs write enable

void w25n01_write_status_register(
    SPI_DEVICE_PARAM, w25n01_status_register_t addr,
    uint8_t value);  // write a value to a status register (protection, config,
                     // status)
uint8_t w25n01_read_status_register(
    SPI_DEVICE_PARAM,
    w25n01_status_register_t addr);  // read a status register

void bbm_add(
    SPI_DEVICE_PARAM, uint16_t logical_block_address,
    uint16_t physical_block_address);  // note it's block address, not page
                                       // address, needs write enable
void bbm_read(
    SPI_DEVICE_PARAM,
    w25n01_bbm_entry* entries);  // read the bbm lut into the entries array,
                                 // array must be 20 entries long

// This is used for instructions that only need the instruction byte
#define __INSTR_SINGLE(name, ins)                        \
    static inline void w25n01_##name(SPI_DEVICE_PARAM) { \
        uint8_t src[] = {ins};                           \
        w25n01_write(spi, src, 1);                       \
    }

// For a group of instructions that deal with page address
#define __INSTR_PAGE(name, ins)                                         \
    static inline void w25n01_##name(SPI_DEVICE_PARAM, uint16_t addr) { \
        uint8_t src[4] = {ins, 0, addr >> 8, addr & 0xFF};              \
        w25n01_write(spi, src, 4);                                      \
    }

__INSTR_SINGLE(reset,
               w25n01_ins_reset);  // reset the chip, erases chip's buffer
__INSTR_SINGLE(write_enable, w25n01_ins_write_enable);    // enable writes
__INSTR_SINGLE(write_disable, w25n01_ins_write_disable);  // disable writes

__INSTR_PAGE(read_page, w25n01_ins_page_data_read);  // reads page into buffer
__INSTR_PAGE(
    write_page,
    w25n01_ins_program_execute);  // writes buffer to page, needs write enable
__INSTR_PAGE(block_erase,
             w25n01_ins_block_erase);  // erases block based on first page
                                       // address of block, needs write enable

#undef __INSTR_PAGE
#undef __INSTR_SINGLE

/*
 * Status Register Bitfields
 */

// multi length bitfield
#define __RREG_BITFIELD(name, addr, mask, shift)                             \
    static inline uint8_t w25n01_get_##name(SPI_DEVICE_PARAM) {              \
        return (w25n01_read_status_register(spi, addr) & (mask)) >> (shift); \
    }

// boolean bitfield
#define __RREG_BITFIELD_BOOL(name, addr, bit)                         \
    static inline bool w25n01_get_##name(SPI_DEVICE_PARAM) {          \
        return w25n01_read_status_register(spi, addr) & (1 << (bit)); \
    }

#define __WREG_BITFIELD_BOOL(name, addr, bit)                            \
    static inline void w25n01_set_##name(SPI_DEVICE_PARAM, bool value) { \
        uint8_t reg = w25n01_read_status_register(spi, addr);            \
        if (value) {                                                     \
            reg |= (1 << (bit));                                         \
        } else {                                                         \
            reg &= ~(1 << (bit));                                        \
        }                                                                \
        w25n01_write_status_register(spi, addr, reg);                    \
    }

// the following lines are just for setting bitfields in the status registers
// we're not gonna fuck with any of the OTP lockable areas, so we're not gonna
// define any of those

// SR-2 (Config Register)
__RREG_BITFIELD_BOOL(buffer_mode, w25n01_config_register, 3);
__WREG_BITFIELD_BOOL(buffer_mode, w25n01_config_register, 3);

__RREG_BITFIELD_BOOL(ecc_enable, w25n01_config_register, 4);
__WREG_BITFIELD_BOOL(ecc_enable, w25n01_config_register, 4);

// SR-3 (Status Register)
__RREG_BITFIELD_BOOL(busy, w25n01_status_register, 0);
__RREG_BITFIELD_BOOL(write_enable, w25n01_status_register, 1);
__RREG_BITFIELD_BOOL(erase_fail, w25n01_status_register, 2);
__RREG_BITFIELD_BOOL(program_fail, w25n01_status_register, 3);
__RREG_BITFIELD(ecc_status, w25n01_status_register, 0b110000, 4);
__RREG_BITFIELD_BOOL(bbm_lut_full, w25n01_status_register, 6);

#undef __WREG_BITFIELD_BOOL
#undef __RREG_BITFIELD_BOOL
#undef __RREG_BITFIELD