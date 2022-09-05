#ifndef FLASH_H
#define FLASH_H

#include <stdbool.h>
#include "spi.h"

// Flash Status Registers
#define FLASH_PROTECTION_REG 0xA0
#define FLASH_CONFIG_REG 0xB0
#define FLASH_STATUS_REG 0xC0
#define FLASH_BUSY 0x01

// Flash Commands
#define FLASH_RESET_CMD 0xFF
#define FLASH_ID_CMD 0x9F
#define FLASH_R_STATUS_CMD 0x0F
#define FLASH_W_STATUS_CMD 0x1F
#define FLASH_W_EN_CMD 0x06
#define FLASH_W_DIS_CMD 0x04
#define FLASH_W_BBM_CMD 0xA1
#define FLASH_R_BBM_CMD 0xA5
#define FLASH_ECC_FAIL_CMD 0xA9
#define FLASH_BLOCK_ERASE_CMD 0xD8
#define FLASH_PROGRAM_LOAD_CMD 0x02
#define FLASH_RAND_PROGRAM_CMD 0x84
#define FLASH_PROGRAM_EXEC_CMD 0x10
#define FLASH_PAGE_READ_CMD 0x13
#define FLASH_READ_CMD 0x03
#define FLASH_FAST_READ 0x0B

// Other useful flash constants
#define FLASH_COL_SIZE 0x7FF
#define FLASH_MAX_PAGE 0xFFFF
#define FLASH_BBM_LEN 20

typedef struct {
  spi_slave_t slave;
  uint16_t page;
  uint16_t column;
} flash_t;

flash_t flash_init(volatile uint8_t *, volatile uint8_t *, uint8_t);
void flash_reset(flash_t);
void flash_enable(flash_t);
void flash_disable(flash_t);
void flash_write_bbm(flash_t, uint16_t, uint16_t);
void flash_read_bbm(flash_t, uint16_t *);
uint8_t flash_read_status(flash_t, uint8_t);
void flash_write_status(flash_t, uint8_t, uint8_t);
void flash_block_erase(flash_t, uint16_t);
void flash_load_program(flash_t *, void *, uint8_t, bool);
void flash_program_exec(flash_t *);
void flash_command(flash_t, uint8_t *, uint8_t *, uint8_t);
bool flash_isbusy(flash_t);

#endif