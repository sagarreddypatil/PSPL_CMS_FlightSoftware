#include "ads13x.h"

#include <stdio.h>
#include <string.h>
#include <myspi.h>

#define NUM_CHANNELS   6    
#define WORD_SIZE      3
#define WORD_SIZE_INIT 3

#define TRANSFER_WORDS (NUM_CHANNELS + 2)  // status word + crc word

#define TRANSFER_SIZE      (TRANSFER_WORDS * WORD_SIZE)
#define TRANSFER_SIZE_INIT (TRANSFER_WORDS * WORD_SIZE_INIT)

// Commands
#define RESET   0b10001
#define STANDBY 0b100010
#define WAKEUP  0b0000000000110011
#define LOCK    0b0000010101010101
#define UNLOCK  0b0000011001010101
#define RREG    0b1010000000000000
#define WREG    0b0110000000000000

// Command Responses
#define RESET_RESP   (0xff26)
#define STANDBY_RESP 0b0000000000100010
#define WAKEUP_RESP  0b0000000000110011
#define LOCK_RESP    0b0000010101010101
#define UNLOCK_RESP  0b0000011001010101
#define RREG_RESP    0b1110000000000000
#define WREG_RESP    0b0100000000000000

#define REG_ADDR_MASK  (1 << 6) - 1
#define REG_ADDR_SHIFT 7

#define REG_NUM_MASK  (1 << 7) - 1
#define REG_NUM_SHIFT 0

// Mask and shift macro
#define MS(val, mask, shift) (((val) & (mask)) << (shift))  // for multiple bits
#define S(val, shift)        ((val & 1) << (shift))         // for single bit

// num - 1 because 0 is 1 reg, don't pass 0, chip designers are dumb
#define REG_OP(op, reg, num)                       \
    (op | MS(reg, REG_ADDR_MASK, REG_ADDR_SHIFT) | \
     MS((num - 1), REG_NUM_MASK, REG_NUM_SHIFT))

#define REG_OP_SINGLE(op, reg) (op | MS(reg, REG_ADDR_MASK, REG_ADDR_SHIFT))

// Host byte-order to peripheral, 16, 24, 32 bit
// Remember to pad LSBs for data types shorter than the set word size
#define HTOP16(val) ((val >> 8) & 0xFF), (val & 0xFF)
#define HTOP24(val) ((val >> 16) & 0xFF), ((val >> 8) & 0xFF), (val & 0xFF)
#define HTOP32(val)                                                  \
    ((val >> 24) & 0xFF), ((val >> 16) & 0xFF), ((val >> 8) & 0xFF), \
        (val & 0xFF)

// Peripheral byte-order to Host, 16, 24, 32 bit
#define PTOH16(ptr) ((ptr)[0] << 8 | (ptr)[1])  // short, swapping bytes
#define PTOH24(ptr)                                       \
    (((ptr)[0] << 24 | (ptr)[1] << 16 | (ptr)[2] << 8) >> \
     8)  // sign extended 32-bit
#define PTOH32(ptr)                                    \
    ((ptr)[0] << 24 | (ptr)[1] << 16 | (ptr)[2] << 8 | \
     (ptr)[3])  // just byte order swapping

#define CRC_POLYNOMIAL 0x1021

uint16_t crc_ccitt_byte(uint16_t crc, uint8_t data) {
    crc ^= (uint16_t)data << 8; // XOR CRC with data
    for (int i = 0; i < 8; i++) {
        if (crc & 0x8000) {
            crc = (crc << 1) ^ CRC_POLYNOMIAL; // XOR CRC with polynomial if MSB is set
        } else {
            crc <<= 1; // Shift CRC left by 1 bit
        }
    }
    return crc;
}

uint16_t calculate_crc_ccitt(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;  // Seed value
    while (length--) {
        crc = crc_ccitt_byte(crc, *data++);
    }
    return crc;
}

void ads13x_reset(SPI_DEVICE_PARAM) {
    // try both 24-bit and 32-bit word size

    // uint8_t src24[TRANSFER_SIZE] = {HTOP16(RESET)};
    // uint8_t dst24[TRANSFER_SIZE];

    // SPI_TRANSFER(src24, dst24, TRANSFER_SIZE);

    uint8_t src[TRANSFER_SIZE_INIT] = {HTOP16(RESET), 0};
    uint8_t dst[TRANSFER_SIZE_INIT];

    SPI_TRANSFER(spi, src, dst, TRANSFER_SIZE_INIT);
}

bool ads13x_ready(SPI_DEVICE_PARAM) {
    // ready waits for chip to start after a reset
    uint8_t dst[TRANSFER_SIZE_INIT];

    SPI_READ(spi, dst, TRANSFER_SIZE_INIT);

    uint16_t status = PTOH16(dst);

    return (status & 0xFF00) == 0x0500;  // check status is init state
}

bool ads13x_init(SPI_DEVICE_PARAM) {
    // reset the chip
    {
        uint8_t src[TRANSFER_SIZE_INIT] = {HTOP16(RESET), 0};
        SPI_WRITE(spi, src, TRANSFER_SIZE_INIT);

        // verify reset
        uint8_t dst[TRANSFER_SIZE_INIT];
        SPI_READ(spi, dst, TRANSFER_SIZE_INIT);

        uint16_t status = PTOH16(dst);
        // safeprintf("1. %x -- %x\n", status, 0x11);
        if (status != RESET_RESP) {
            // return false;  // TODO: fix this to compare the correct values
        }
    }

    const uint16_t mode_reg_value =
        // 0x0500 | (0b01 << 8);  // set WLENGTH to 0b01
        0b0001000100000100;
    {
        // set the mode register
        const uint16_t opcode = REG_OP_SINGLE(WREG, ads13x_mode);

        uint8_t src[TRANSFER_SIZE_INIT] = {HTOP16(opcode), 0,
                                           HTOP16(mode_reg_value), 0};
        uint8_t dst[TRANSFER_SIZE_INIT] = {0};
        SPI_WRITE(spi, src, TRANSFER_SIZE_INIT);

        SPI_READ(spi, dst, TRANSFER_SIZE_INIT);

        uint16_t resp     = PTOH16(dst);
        uint16_t expected = REG_OP_SINGLE(WREG_RESP, ads13x_mode);

        if (resp != expected) {
            return false;
        }
    }

    {
        // read the mode register
        const uint16_t opcode = REG_OP_SINGLE(RREG, ads13x_mode);

        uint8_t src[TRANSFER_SIZE] = {HTOP16(opcode), 0};
        uint8_t dst[TRANSFER_SIZE] = {0};

        SPI_WRITE(spi, src, TRANSFER_SIZE);
        SPI_READ(spi, dst, TRANSFER_SIZE);

        uint16_t resp = PTOH16(dst);
        // safeprintf("3. %x -- %x\n", resp, mode_reg_value);
        if (resp != mode_reg_value) { 
            // return false;  // TODO: fix this to compare the correct values
        }
    }

    return true;
}

bool ads13x_wreg_single(SPI_DEVICE_PARAM, ads13x_reg_t reg, uint16_t data) {
    uint8_t src[TRANSFER_SIZE] = {
        HTOP16(REG_OP_SINGLE(WREG, reg)), 0, HTOP16(data), 0};
    uint8_t dst[TRANSFER_SIZE];

    SPI_WRITE(spi, src, TRANSFER_SIZE);
    SPI_READ(spi, dst, TRANSFER_SIZE);

    uint16_t resp     = (dst[0] << 8) | dst[1];
    uint16_t expected = REG_OP_SINGLE(WREG_RESP, reg);

    // safeprintf("%x %x\n", resp, expected); // TODO: not returning expected, ever

    return resp == expected;
}

uint16_t ads13x_rreg_single(SPI_DEVICE_PARAM, ads13x_reg_t reg) {
    uint8_t src[TRANSFER_SIZE * 2] = {HTOP16(REG_OP_SINGLE(RREG, reg))};
    uint8_t dst[TRANSFER_SIZE * 2] = {0};

    SPI_WRITE(spi, src, TRANSFER_SIZE * 2);
    SPI_READ(spi, dst, TRANSFER_SIZE * 2);

    uint16_t resp = (dst[TRANSFER_SIZE] << 8) | dst[TRANSFER_SIZE + 1];
    return resp;
}

void ads13x_set_sample_rate(SPI_DEVICE_PARAM, ads13x_sample_rate sample_rate) {
    uint16_t clock_reg = ads13x_rreg_single(spi, ads13x_clock);
    clock_reg |= MS(sample_rate, 0b111, 2);

    ads13x_wreg_single(spi, ads13x_clock, clock_reg);
}

bool ads13x_read_data(SPI_DEVICE_PARAM, uint16_t *status, int32_t *data,
                      uint32_t len) {
    if (len > NUM_CHANNELS) len = NUM_CHANNELS;

    uint8_t dst[TRANSFER_SIZE];
    SPI_READ(spi, dst, TRANSFER_SIZE);

    uint16_t crc_local = calculate_crc_ccitt(dst, TRANSFER_SIZE - WORD_SIZE);

    // first 2 of last 3 bytes
    uint16_t crc_spi = (dst[TRANSFER_SIZE - WORD_SIZE + 1]) + (dst[TRANSFER_SIZE - WORD_SIZE] << 8);

    // for (size_t i = 0; i < TRANSFER_SIZE; i++) {
    //     printf("%02x ", dst[i]);
    // }
    // printf("\n");

    if (crc_local != crc_spi) {
        // printf("crc mismatch: %04x != %04x\n", crc_local, crc_spi);
        return false;
    }

    *status = PTOH16(dst);
    for (int i = 0; i < len; i++) {
        data[i] = (dst[WORD_SIZE + (i * WORD_SIZE)] << 16) + (dst[WORD_SIZE + (i * WORD_SIZE) + 1] << 8) + (dst[WORD_SIZE + (i * WORD_SIZE) + 2]);
    }

    return true;
}
