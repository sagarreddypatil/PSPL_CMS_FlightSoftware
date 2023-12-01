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

#define DEBUG_TRANSFER 1

static TaskHandle_t xTaskToNotify = NULL;

void spi_device_init(spi_device_t *spi, uint8_t miso_gpio, uint8_t mosi_gpio, uint8_t sck_gpio)
{
	// Initialize SPI pins
    gpio_set_function(miso_gpio, GPIO_FUNC_SPI);
    gpio_set_function(mosi_gpio, GPIO_FUNC_SPI);
    gpio_set_function(sck_gpio, GPIO_FUNC_SPI);

    gpio_init(spi->cs_gpio);
    gpio_put(spi->cs_gpio, 1);
	gpio_set_dir(spi->cs_gpio, GPIO_OUT);

    gpio_set_slew_rate(miso_gpio, GPIO_SLEW_RATE_FAST);
	gpio_set_slew_rate(mosi_gpio, GPIO_SLEW_RATE_FAST);
	gpio_set_slew_rate(sck_gpio, GPIO_SLEW_RATE_FAST);

	gpio_set_drive_strength(miso_gpio, GPIO_DRIVE_STRENGTH_4MA);
	gpio_set_drive_strength(mosi_gpio, GPIO_DRIVE_STRENGTH_4MA);
	gpio_set_drive_strength(sck_gpio, GPIO_DRIVE_STRENGTH_4MA);

    spi->dma_channel_1 = dma_claim_unused_channel(true);
    spi->dma_channel_2 = dma_claim_unused_channel(true);
    spi->baudrate = spi_init(spi1, spi->baudrate);

}

void spi_write(spi_device_t *spi, uint8_t *src, size_t size)
{   

    gpio_put(spi->cs_gpio, 0);
    spi_dma_transfer(spi, src, NULL, size);
    gpio_put(spi->cs_gpio, 1);
}

void spi_write_read(spi_device_t *spi, uint8_t *src, uint8_t *dst, size_t size)
{

    gpio_put(spi->cs_gpio, 0);
    spi_dma_transfer(spi, src, dst, size);
    gpio_put(spi->cs_gpio, 1);

}

void spi_dma_transfer(spi_device_t *spi, volatile void *src, volatile void *dst, size_t size)
{
    uint channel_1 = dma_claim_unused_channel(true);
    uint channel_2 = dma_claim_unused_channel(true);
    dma_channel_config config_1 = dma_channel_get_default_config(channel_1);
    dma_channel_config config_2 = dma_channel_get_default_config(channel_2);

    // Sets number of bits to move per transfer
    channel_config_set_transfer_data_size(&config_1, DMA_TRANSFER_SIZE);

    // Sets read/write pointers to increment by DMA_TRANSFER_SIZE after each transfer (or not)
    channel_config_set_read_increment(&config_1, true);
	channel_config_set_write_increment(&config_1, false);

    // Set pacing using given DREQ (data request)
    channel_config_set_dreq(&config_1, spi_get_dreq(spi->spi_bus->bus, true));
    dma_channel_configure(
        channel_1,
        &config_1,
        &spi_get_hw(spi->spi_bus->bus)->dr,
        src,
        size,
        false
    );
    

    channel_config_set_transfer_data_size(&config_2, DMA_TRANSFER_SIZE);

    channel_config_set_read_increment(&config_2, false);
	channel_config_set_write_increment(&config_2, true);

    channel_config_set_dreq(&config_2, spi_get_dreq(spi->spi_bus->bus, false));
    dma_channel_configure(
        channel_2,
        &config_2,
        dst,
        &spi_get_hw(spi->spi_bus->bus)->dr,
        size,
        false
    );

    dma_start_channel_mask((1u << channel_1) | (1u << channel_2));

    xTaskToNotify = xTaskGetCurrentTaskHandle();
	uint32_t ulNotificationValue;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS( 200 );

	ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);

	if( ulNotificationValue == 1 )
	{
		dma_unclaim_mask((1u << channel_1) | (1u << channel_2));
	}
	else
	{
        dma_unclaim_mask((1u << channel_1) | (1u << channel_2));
	}


    // dma_channel_wait_for_finish_blocking(channel_1);
    // dma_channel_wait_for_finish_blocking(channel_2);
    // dma_unclaim_mask((1u << channel_1) | (1u << channel_2));

#if DEBUG_TRANSFER
    printf("\nwrite: ");
    for (int i = 0; i < size; i++) {
        printf("%02x ", ((uint8_t*)src)[i]);
    }
    printf("\nread: ");
    for (int i = 0; i < size; i++) {
        printf("%02x ", ((uint8_t*)dst)[i]);
    }
    printf("\n");
#endif
}

void dma_finished_isr()
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	vTaskNotifyGiveFromISR(xTaskToNotify, &xHigherPriorityTaskWoken );
	xTaskToNotify = NULL;

    for (uint8_t i = 0; i < 12; i++)
    {
        if (dma_channel_get_irq0_status(i))
        {
            dma_channel_acknowledge_irq0(i);
            break;
        }
    }
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}