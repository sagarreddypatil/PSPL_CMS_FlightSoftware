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

#define DMA_TRANSFER_SIZE DMA_SIZE_8

// Contains address of and mutex for an spi bus
typedef struct {
  spi_inst_t *bus;
  uint8_t index;

  // must be statically created with a StaticSemaphore_t buffer
  SemaphoreHandle_t mutex;

} spi_bus_t;

// Contains all info about a specific SPI device
typedef struct {
  uint8_t cs_gpio;
  uint baudrate;
  spi_cpol_t cpol;
  spi_cpha_t cpha;

  spi_bus_t *spi_bus;

  uint dma_channel_1;
  uint dma_channel_2;

} spi_device_t;

/*! @brief initialize an SPI device, writing configuration to provided struct
 *   @param spi SPI struct that will be configured
 *   @param miso_gpio
 *   @param mosi_gpio
 *   @param sck_gpio
 */
void spi_device_init(spi_device_t *spi, uint8_t miso_gpio, uint8_t mosi_gpio,
                     uint8_t sck_gpio);

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
void spi_dma_transfer(spi_device_t *spi, volatile void *src, volatile void *dst,
                      size_t size);

/*! \brief configures the SPI bus for a specific device
 * \param spi
 * \return actual baudrate
 */
uint myspi_configure(spi_device_t *spi);

void dma_finished_isr0();
void dma_finished_isr1();

#define SPI_DEVICE_PARAM spi_device_t *spi

#define SPI_TRANSFER(src, dst, len) spi_write_read(spi, src, dst, len)

#define SPI_WRITE(src, len) \
  uint8_t __dst[len];       \
  spi_write_read(spi, src, __dst, len)

#define SPI_READ(dst, len) \
  uint8_t __src[len];      \
  spi_write_read(spi, __src, dst, len)
