#ifndef FLASH_H
#define FLASH_H

#include "spi.h"

#define WRITE_ENABLE 0x06
#define WRITE_DISABLE 0x04

void flash_init(volatile uint8_t*, volatile uint8_t*, uint8_t*);
void flash_write(volatile uint8_t*);

#endif