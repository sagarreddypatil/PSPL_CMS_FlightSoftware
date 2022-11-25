#pragma once
#include <stdlib.h>
#include <stdint.h>


typedef struct spi_inst spi_inst_t;

#define spi0 0
#define spi1 1

 typedef enum {
     SPI_CPHA_0 = 0,
     SPI_CPHA_1 = 1
 } spi_cpha_t;
  
 typedef enum {
     SPI_CPOL_0 = 0,
     SPI_CPOL_1 = 1
 } spi_cpol_t;
  
 typedef enum {
     SPI_LSB_FIRST = 0,
     SPI_MSB_FIRST = 1
 } spi_order_t;

void print8 (const uint8_t* arr, size_t len);
void print16 (const uint16_t* arr, size_t len);
void spi_set_format (spi_inst_t * spi, uint data_bits, spi_cpol_t cpol, spi_cpha_t cpha, spi_order_t order);
int spi_set_baudrate(spi_inst_t * spi, uint baud);
int spi_write_read_blocking	(spi_inst_t * spi, const uint8_t * src, uint8_t * dst, size_t len);
int spi_write16_read16_blocking	(spi_inst_t * spi, const uint16_t * src, uint16_t * dst, size_t len);
int spi_write_blocking	(spi_inst_t * spi, const uint8_t * src, size_t len);
int spi_read_blocking (spi_inst_t * spi, uint8_t repeated_tx_data, uint8_t * dst, size_t len);
int spi_write16_blocking (spi_inst_t * spi, const uint16_t * src, size_t len);
int spi_read16_blocking (spi_inst_t * spi, uint16_t repeated_tx_data, uint16_t * dst, size_t len);










































































































