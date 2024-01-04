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

#define DMA_DEFAULT_TRANSFER_SIZE DMA_SIZE_8
#define myspi0                    (myspi_t *)&myspi_bus_0
#define myspi1                    (myspi_t *)&myspi_bus_1

#define SPI_DEVICE_PARAM myspi_device_t *spi

#define SPI_TRANSFER(spi, src, dst, len) myspi_write_read(spi, src, dst, len)

#define SPI_WRITE(spi, src, len) \
    uint8_t __dst[len];          \
    myspi_write_read(spi, src, __dst, len)

#define SPI_READ(spi, dst, len) \
    uint8_t __src[len];         \
    myspi_write_read(spi, __src, dst, len)

// Type representing SPI bus, only needs spi_inst set
typedef struct {
    spi_inst_t *spi_inst;
    // 0 or 1 representing SPI instance (spi_get_index())
    uint8_t index;

    volatile TaskHandle_t current_task;
    StaticSemaphore_t mutex_buf;
    SemaphoreHandle_t mutex;

    uint8_t dma_rx;
    uint8_t dma_tx;

    dma_channel_config rx_config;
    dma_channel_config tx_config;
} myspi_t;

extern volatile myspi_t myspi_bus_0;
extern volatile myspi_t myspi_bus_1;

// Type representing SPI device
typedef struct {
    uint8_t cs_gpio;
    uint baudrate;

    spi_cpol_t cpol;
    spi_cpha_t cpha;

    myspi_t *spi_bus;

} myspi_device_t;

/*! @brief initialize an SPI device, writing configuration to provided struct
 *  @param spi pointer to SPI device struct that will be configured
 *  @param spi pointer to SPI bus struct that will be configured
 *  @param cs_gpio
 *  @param miso_gpio
 *  @param mosi_gpio
 *  @param sck_gpio
 *  @param cpol clock polarity
 *  @param cpha clock phase
 *  @param baudrate
 */
void myspi_device_init(myspi_device_t *spi, myspi_t *spi_bus, uint8_t cs_gpio,
                       uint8_t miso_gpio, uint8_t mosi_gpio, uint8_t sck_gpio,
                       spi_cpol_t cpol, spi_cpha_t cpha, uint baudrate);

/*! @brief initalize an SPI bus, writing configuration to provided struct
 *  @param bus pointer to SPI bus struct that will be configured
 *  @param mutex_buffer pointer to mutex buffer
 */
void myspi_bus_init(myspi_t *bus);

/*! @brief Write from a buffer to an SPI device
 *  @param device SPI device that will be written to
 *  @param src address of buffer that will be read from
 *  @param dst address of buffer that will be written to
 *  @param size size of buffers (BOTH) in bytes
 */
void myspi_write_read(myspi_device_t *spi, uint8_t *src, uint8_t *dst,
                      size_t size);

/*! @brief performs a DMA transfer to and from a data register with a DREQ as
 * pacing
 *  @param spi
 *  @param src memory address of what will be written to the data register (dr)
 *  @param dst memory address that received data will be written to
 *  @param size size of transfer in bytes (max between src and dst)
 */
void myspi_dma_transfer(myspi_device_t *spi, volatile void *src,
                        volatile void *dst, size_t size);

/*! @brief configures the SPI bus for a specific device
 *  @param spi
 *  @return actual baudrate
 */
uint myspi_configure(myspi_device_t *spi);

void dma_finished_isr0();
void dma_finished_isr1();
