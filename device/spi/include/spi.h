#pragma once

// FreeRTOS Includes
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

// RP2040 Includes
#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <hardware/dma.h>
#include <hardware/irq.h>

#define DMA_TRANSFER_SIZE DMA_SIZE_8

// #define PICOTOOL_ENABLED 1

/*! @brief macro to make calling spi_write_read easier
 *  @param spi_device SPI device to write to, of type spi_device_t
 *  @param src 
 *  @param dst
 *  @param size
 */
#define SPI_WRITE_READ(spi_device, src, dst, size)          \
  _Generic((src),                                           \
    uint8_t*:  spi_write_read8,                             \
    uint16_t*: spi_write_read16,                            \
    uint32_t*: spi_write_read32)(spi_device, src, dst, size);

// Contains all info about a specific SPI device (TYPES NEED OPTIMIZED, probably)
typedef struct {
  
  spi_inst_t *spi_inst;
  const uint8_t miso_gpio;
  const uint8_t mosi_gpio;
  const uint8_t sck_gpio;
  const uint8_t cs_gpio;
  uint baudrate;     // is not guaranteed, actual is set when device initialized

  irq_handler_t isr; // Interrupt service routine function, could become a union
  uint tx_dma;       // with variables for a common function
  uint rx_dma;
  dma_channel_config tx_dma_config;
  dma_channel_config rx_dma_config;
  TaskHandle_t task;
  SemaphoreHandle_t semphr; 

} spi_device_t;

/*! @brief initialize an SPI device, writing configuration to provided struct
*   @param device SPI struct that will be configured
*/
void spi_device_init(spi_device_t *device);

/*! @brief Write from a buffer to an SPI device
*   @param device SPI device that will be written to
*   @param src address of buffer that will be read from
*   @param size size of buffer in bytes
*/
void spi_write(spi_device_t *device, uint32_t *src, size_t size);

/*! @brief Write from a buffer to an SPI device
*   @param device SPI device that will be written to
*   @param src address of buffer that will be read from
*   @param dst address of buffer that will be written to
*   @param size size of buffers (BOTH) in bytes
*/
void spi_write_read16(spi_device_t *device, uint16_t *src, uint16_t *dst, size_t size);

/*! @brief Write from a buffer to an SPI device
*   @param device SPI device that will be written to
*   @param src address of buffer that will be read from
*   @param dst address of buffer that will be written to
*   @param size size of buffers (BOTH) in bytes
*/
void spi_write_read8(spi_device_t *device, uint8_t *src, uint8_t *dst, size_t size);

/*! @brief Write from a buffer to an SPI device
*   @param device SPI device that will be written to
*   @param src address of buffer that will be read from
*   @param dst address of buffer that will be written to
*   @param size size of buffers (BOTH) in bytes
*/
void spi_write_read32(spi_device_t *device, uint32_t *src, uint32_t *dst, size_t size);


void spi_irq_handler(spi_device_t *device);

void dma_move(uint8_t *src, uint8_t *dst, size_t size);