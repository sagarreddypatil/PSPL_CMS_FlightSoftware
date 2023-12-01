#pragma once

// FreeRTOS Includes
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

// RP2040 Includes
#include <hardware/spi.h>
#include <hardware/dma.h>
#include <hardware/irq.h>

#include <mydma.h>

#define SPI_DEVICE_PARAM (spi_device_t *spi)

#define DMA_TRANSFER_SIZE DMA_SIZE_8

// Contains address of and mutex for an spi bus
typedef struct {

  spi_inst_t *bus;

  // must be statically created with a StaticSemaphore_t buffer
  SemaphoreHandle_t mutex; 

} spi_bus_t;


// Contains all info about a specific SPI device
typedef struct {
  
  uint8_t cs_gpio;
  uint baudrate;

  spi_bus_t *spi_bus;

  uint8_t dma_channel_1;
  uint8_t dma_channel_2;

} spi_device_t;


/*! @brief initialize an SPI device, writing configuration to provided struct 
*   @param spi SPI struct that will be configured
*   @param miso_gpio
*   @param mosi_gpio
*   @param sck_gpio
*/
void spi_device_init(spi_device_t *spi, uint8_t miso_gpio, uint8_t mosi_gpio, uint8_t sck_gpio);

/*! @brief Write from a buffer to an SPI device
*   @param device SPI device that will be written to
*   @param src address of buffer that will be read from
*   @param size size of buffer in bytes
*/
void spi_write(spi_device_t *spi, uint8_t *src, size_t size);

/*! @brief Write from a buffer to an SPI device
*   @param device SPI device that will be written to
*   @param src address of buffer that will be read from
*   @param dst address of buffer that will be written to
*   @param size size of buffers (BOTH) in bytes
*/
void spi_write_read(spi_device_t *spi, uint8_t *src, uint8_t *dst, size_t size);

/*! \brief performs a DMA transfer to and from a data register with a DREQ as
 * pacing
 * \param spi 
 * \param src memory address of what will be written to the data register (dr)
 * \param dst memory address that received data will be written to
 * \param size size of transfer in bytes (max between src and dst)
 */
void spi_dma_transfer(spi_device_t *spi, volatile void *src, volatile void *dst, size_t size);


void dma_finished_isr();
