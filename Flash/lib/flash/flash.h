#ifndef FLASH_H
#define FLASH_H

#include <stdbool.h>
#include "spi.h"

// Flash SPI Instructions
#define RESET_COMMAND 0xFF
#define WRITE_ENABLE 0x06
#define WRITE_DISABLE 0x04
#define LOAD_PROGRAM 0x02
#define RANDOM_LOAD_PROGRAM 0x84

// Other useful constants
#define UINT16_MAX 65535

typedef struct{
  spi_slave_t slave;
  uint16_t page;
  uint16_t column;
} flash_t;

void flash_reset(flash_t);

#endif