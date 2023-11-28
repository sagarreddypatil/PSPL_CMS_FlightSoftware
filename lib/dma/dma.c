#include <stdio.h>
#include <dma.h>
#include <hardware/regs/dreq.h>
#include <hardware/dma.h>
#include <FreeRTOS.h>

#define DEBUG_TRANSFER 0

static TaskHandle_t xTaskToNotify = NULL;

void dmacpy(volatile void *dst, volatile void *src, size_t size)
{
	uint channel = dma_claim_unused_channel(true);
	dma_channel_config config = dma_channel_get_default_config(channel);

    // Sets number of bits to move per transfer
	channel_config_set_transfer_data_size(&config, DMA_TRANSFER_SIZE);

    // Sets read/write pointers to increment by DMA_TRANSFER_SIZE after each transfer
	channel_config_set_read_increment(&config, true);
	channel_config_set_write_increment(&config, true);

    dma_channel_configure(
        channel,
        &config,
        dst,
        src,
        size,
        true
    );

	dma_channel_wait_for_finish_blocking(channel);
    dma_unclaim_mask(1 << channel);
}

void dmacpy4(volatile void *dst, volatile void *src, size_t size)
{
	uint channel = dma_claim_unused_channel(true);
	dma_channel_config config = dma_channel_get_default_config(channel);

    // Sets number of bits to move per transfer
	channel_config_set_transfer_data_size(&config, DMA_SIZE_32);

    // Sets read/write pointers to increment by DMA_TRANSFER_SIZE after each transfer
	channel_config_set_read_increment(&config, true);
	channel_config_set_write_increment(&config, true);

    dma_channel_configure(
        channel,
        &config,
        dst,
        src,
        size/4,
        true
    );

	dma_channel_wait_for_finish_blocking(channel);
    dma_unclaim_mask(1 << channel);
}

void dmacpy2(volatile void *src1, volatile void *src2, volatile void *dst1, volatile void *dst2, size_t size)
{
    uint channel_1 = dma_claim_unused_channel(true);
    uint channel_2 = dma_claim_unused_channel(true);
    dma_channel_config config = dma_channel_get_default_config(channel_1);

    // Sets number of bits to move per transfer
    channel_config_set_transfer_data_size(&config, DMA_TRANSFER_SIZE);

    // Sets read/write pointers to increment by DMA_TRANSFER_SIZE after each transfer
    channel_config_set_read_increment(&config, true);
	channel_config_set_write_increment(&config, true);

    dma_channel_configure(
        channel_1,
        &config,
        dst1,
        src1,
        size,
        false
    );
    dma_channel_configure(
        channel_2,
        &config,
        dst2,
        src2,
        size,
        false
    );
    dma_start_channel_mask((1u << channel_1) | (1u << channel_2));

    dma_channel_wait_for_finish_blocking(channel_2);
    dma_channel_wait_for_finish_blocking(channel_1);
    dma_unclaim_mask((1u << channel_1) | (1u << channel_2));
}

void dmatransfer(volatile void *src, io_rw_32 *dr, volatile void *dst, size_t size, uint tx_dreq, uint rx_dreq)
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
    channel_config_set_dreq(&config_1, tx_dreq);
    dma_channel_configure(
        channel_1,
        &config_1,
        dr,
        src,
        size,
        false
    );
    

    channel_config_set_transfer_data_size(&config_2, DMA_TRANSFER_SIZE);

    channel_config_set_read_increment(&config_2, false);
	channel_config_set_write_increment(&config_2, true);

    channel_config_set_dreq(&config_2, rx_dreq);
    dma_channel_configure(
        channel_2,
        &config_2,
        dst,
        dr,
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

void dma_write(volatile void *src, io_rw_32 *dr, size_t size, uint dreq)
{
    uint channel_1 = dma_claim_unused_channel(true);
    dma_channel_config config_1 = dma_channel_get_default_config(channel_1);

    // Sets number of bits to move per transfer
    channel_config_set_transfer_data_size(&config_1, DMA_TRANSFER_SIZE);

    // Sets read/write pointers to increment by DMA_TRANSFER_SIZE after each transfer (or not)
    channel_config_set_read_increment(&config_1, true);
	channel_config_set_write_increment(&config_1, false);

    // Set pacing using given DREQ (data request)
    channel_config_set_dreq(&config_1, dreq);

    dma_channel_configure(
        channel_1,
        &config_1,
        dr,
        src,
        size,
        false
    );
    
    dma_start_channel_mask(1u << channel_1);

    dma_channel_wait_for_finish_blocking(channel_1);
    dma_unclaim_mask(1u << channel_1);
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