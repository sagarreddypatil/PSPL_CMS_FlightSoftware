#ifndef FLASH_H
#define FLASH_H

#include <stdbool.h>
#include "spi.h"

// Flash SPI Instructions
#define FLASH_RESET_COMMAND 0xFF
#define FLASH_WRITE_ENABLE 0x06
#define FLASH_WRITE_DISABLE 0x04
#define FLASH_LOAD_PROGRAM 0x02
#define FLASH_RANDOM_LOAD_PROGRAM 0x84
#define FLASH_READ_STATUS_COMMAND 0x0F
#define FLASH_WRITE_STATUS_COMMAND 0x0F

// Other useful constants
#define UINT16_MAX 0xFFFF

typedef struct{
  spi_slave_t slave;
  uint16_t page;
  uint16_t column;
} flash_t;

void flash_reset(flash_t);

#endif