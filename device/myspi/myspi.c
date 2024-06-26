// FreeRTOS Includes
#include <FreeRTOS.h>

// RP2040 Includes
#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <hardware/regs/dreq.h>
#include <hardware/dma.h>

#include <myspi.h>
#include <stdio.h>
#include <pico/time.h>
#include <pico/mutex.h>

#include <mydma.h>

// #define DEBUG_TRANSFER

#ifdef DEBUG_TRANSFER
#include "semphr.h"
extern SemaphoreHandle_t print_mutex;
void safeprintf(const char *format, ...);
#endif

volatile myspi_t myspi_bus_0;
volatile myspi_t myspi_bus_1;

void myspi_device_init(myspi_device_t *spi, myspi_t *spi_bus, uint8_t cs_gpio,
                       spi_cpol_t cpol, spi_cpha_t cpha, uint baudrate) {
    // Struct configuration
    spi->spi_bus  = spi_bus;
    spi->cs_gpio  = cs_gpio;
    spi->cpol     = cpol;
    spi->cpha     = cpha;
    spi->baudrate = baudrate;

    // Initialize CS pin and set high
    gpio_init(spi->cs_gpio);
    gpio_put(spi->cs_gpio, 1);
    gpio_set_dir(spi->cs_gpio, GPIO_OUT);
}

void myspi_bus_init(myspi_t *bus, uint8_t miso_gpio, uint8_t mosi_gpio,
                    uint8_t sck_gpio) {
    myspi_bus_0.spi_inst = spi0;
    myspi_bus_1.spi_inst = spi1;

    // Initialize SPI pins
    gpio_set_function(miso_gpio, GPIO_FUNC_SPI);
    gpio_set_function(mosi_gpio, GPIO_FUNC_SPI);
    gpio_set_function(sck_gpio, GPIO_FUNC_SPI);

    // Initialize the HW
    spi_init(bus->spi_inst, 1000);  // dummy baudrate, doesn't matter

    /*
     * below rates are default, fast slew not yet tested, need to scope
     * if signal ripples ripples, lower the rate
     * if doesn't rise/fall to correct voltages, raise the rate
     *
     * See:
     * Falstad Circuit Simulator
     * https://tinyurl.com/2m9dxu53
     */

    // gpio_set_slew_rate(miso_gpio, GPIO_SLEW_RATE_FAST);
    // gpio_set_slew_rate(mosi_gpio, GPIO_SLEW_RATE_FAST);
    // gpio_set_slew_rate(sck_gpio, GPIO_SLEW_RATE_FAST);

    // gpio_set_drive_strength(miso_gpio, GPIO_DRIVE_STRENGTH_4MA);
    // gpio_set_drive_strength(moio_gpio, GPIO_DRIVE_STRENGTH_4MA);
    // gpio_set_drive_strength(sck_gpio, GPIO_DRIVE_STRENGTH_4MA);

    // Assign index of SPI instance (0 or 1)
    bus->index = spi_get_index(bus->spi_inst);

    // Assign DMA channels
    bus->dma_rx = dma_claim_unused_channel(true);
    bus->dma_tx = dma_claim_unused_channel(true);

    // Base DMA channel configuration
    bus->rx_config = dma_channel_get_default_config(bus->dma_rx);
    channel_config_set_read_increment(&bus->rx_config, false);
    channel_config_set_write_increment(&bus->rx_config, true);
    channel_config_set_transfer_data_size(&bus->rx_config,
                                          DMA_DEFAULT_TRANSFER_SIZE);
    channel_config_set_dreq(&bus->rx_config,
                            spi_get_dreq(bus->spi_inst, false));
    dma_channel_configure(bus->dma_rx, &bus->rx_config, NULL,
                          &spi_get_hw(bus->spi_inst)->dr, 1, false);

    /****************************************************************************/

    bus->tx_config = dma_channel_get_default_config(bus->dma_tx);
    channel_config_set_transfer_data_size(&bus->tx_config,
                                          DMA_DEFAULT_TRANSFER_SIZE);
    channel_config_set_dreq(&bus->tx_config, spi_get_dreq(bus->spi_inst, true));
    dma_channel_configure(bus->dma_tx, &bus->tx_config,
                          &spi_get_hw(bus->spi_inst)->dr, NULL, 1, false);

    // Disable interrupts on one of the DMA channels (really doesn't matter
    // which) so we don't have overlapping interrupts
    dma_irqn_set_channel_enabled(bus->index, bus->dma_rx, true);
    dma_irqn_set_channel_enabled(bus->index, bus->dma_tx, false);
    // dma_irqn_set_channel_enabled(!bus->index, bus->dma_rx, false);
    // dma_irqn_set_channel_enabled(!bus->index, bus->dma_tx, false);

    // Set interrupt handler depending on which SPI bus
    if (bus->index == 1) {
        irq_set_enabled(DMA_IRQ_1, true);
        irq_set_exclusive_handler(DMA_IRQ_1, dma_finished_isr1);
    } else if (bus->index == 0) {
        irq_set_enabled(DMA_IRQ_0, true);
        irq_set_exclusive_handler(DMA_IRQ_0, dma_finished_isr0);
    }

    // Create and assign mutex
    bus->mutex = xSemaphoreCreateMutexStatic(&bus->mutex_buf);
}

void myspi_write_read(myspi_device_t *spi, uint8_t *src, uint8_t *dst,
                      size_t size) {
    asm volatile("nop \n nop \n nop");
    gpio_put(spi->cs_gpio, 0);
    asm volatile("nop \n nop \n nop");

    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        spi_write_read_blocking(spi->spi_bus->spi_inst, src, dst, size);
    } else {
        myspi_dma_transfer(spi, src, dst, size);
        dma_channel_wait_for_finish_blocking(spi->spi_bus->dma_rx);
        dma_channel_wait_for_finish_blocking(spi->spi_bus->dma_tx);
    }

    asm volatile("nop \n nop \n nop");
    gpio_put(spi->cs_gpio, 1);
    asm volatile("nop \n nop \n nop");
}

void myspi_dma_transfer(myspi_device_t *spi, volatile void *src,
                        volatile void *dst, size_t size) {
    dma_channel_set_write_addr(spi->spi_bus->dma_rx, dst, false);
    dma_channel_set_read_addr(spi->spi_bus->dma_tx, src, false);

    dma_channel_set_trans_count(spi->spi_bus->dma_rx, size, false);
    dma_channel_set_trans_count(spi->spi_bus->dma_tx, size, false);

    if (spi->spi_bus->index == 0) {
        myspi_bus_0.current_task = xTaskGetCurrentTaskHandle();
    } else if (spi->spi_bus->index == 1) {
        myspi_bus_1.current_task = xTaskGetCurrentTaskHandle();
    }

    dma_start_channel_mask((1u << spi->spi_bus->dma_tx) |
                           (1u << spi->spi_bus->dma_rx));

    uint32_t ulNotificationValue;

    ulNotificationValue =
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(portMAX_DELAY));

    assert(ulNotificationValue);

    ulNotificationValue = 0;

#ifdef DEBUG_TRANSFER
    // if (size != 32) return;

    xSemaphoreTake(print_mutex, portMAX_DELAY);
    printf("\nbaudrate: %d", spi->baudrate);

    printf("\nwrite: ");
    for (int i = 0; i < size; i++) {
        printf("%02x ", ((uint8_t *)src)[i]);
    }
    printf("\nread: ");
    for (int i = 0; i < size; i++) {
        printf("%02x ", ((uint8_t *)dst)[i]);
    }
    printf("\n");
    xSemaphoreGive(print_mutex);
#endif
}

uint myspi_configure(myspi_device_t *spi) {
    uint actual_baud = spi_set_baudrate(spi->spi_bus->spi_inst, spi->baudrate);
    spi_set_format(spi->spi_bus->spi_inst, 8, spi->cpol, spi->cpha,
                   SPI_MSB_FIRST);

    return actual_baud;
}

void dma_finished_isr0() {
    portDISABLE_INTERRUPTS();
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    vTaskNotifyGiveFromISR(myspi_bus_0.current_task, &xHigherPriorityTaskWoken);

    hw_set_bits(&dma_hw->ints0, 1u << myspi_bus_0.dma_rx);

    portENABLE_INTERRUPTS();
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void dma_finished_isr1() {
    portDISABLE_INTERRUPTS();
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    vTaskNotifyGiveFromISR(myspi_bus_1.current_task, &xHigherPriorityTaskWoken);

    hw_set_bits(&dma_hw->ints1, 1u << myspi_bus_1.dma_rx);

    portENABLE_INTERRUPTS();
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}