#ifndef FLASH_H
#define FLASH_H

#include "spi.h"

#define WRITE_ENABLE 0x06
#define WRITE_DISABLE 0x04
#define RESET_COMMAND 0xFF
#define JEDEC_COMMAND 0x9F
#define STATUS_COMMAND 0x0F
#define READ_BBM 0xA5

struct spi_device flash_init(volatile uint8_t*, volatile uint8_t*, uint8_t);
void flash_write(struct spi_device);

#endif