#include <stdio.h>
#include <dma.h>
#include <hardware/regs/dreq.h>

void dmacpy(volatile void *src, volatile void *dst, size_t size)
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

void dmatransfer(uint8_t *src, volatile uint32_t *dr, uint8_t *dst, size_t size, uint dreqTX, uint dreqRX)
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
    channel_config_set_dreq(&config_1, DREQ_SPI1_TX);
    dma_channel_configure(
        channel_1,
        &config_1,
        &spi_get_hw(spi1)->dr,
        src,
        size,
        false
    );
    

    channel_config_set_transfer_data_size(&config_2, DMA_TRANSFER_SIZE);

    channel_config_set_read_increment(&config_2, false);
	channel_config_set_write_increment(&config_2, true);

    channel_config_set_dreq(&config_2, DREQ_SPI1_RX);
    dma_channel_configure(
        channel_2,
        &config_2,
        dst,
        &spi_get_hw(spi1)->dr,
        size,
        false
    );

    dma_start_channel_mask((1u << channel_1) | (1u << channel_2));

    dma_channel_wait_for_finish_blocking(channel_2);
    if (dma_channel_is_busy(channel_1)) {
        panic("RX completed before TX");
    }
    dma_unclaim_mask((1u << channel_1) | (1u << channel_2));
}

void dma_write(uint32_t *src, volatile uint32_t *dr, size_t size)
{
    uint channel_1 = dma_claim_unused_channel(true);
    dma_channel_config config_1 = dma_channel_get_default_config(channel_1);

    // Sets number of bits to move per transfer
    channel_config_set_transfer_data_size(&config_1, DMA_TRANSFER_SIZE);

    // Sets read/write pointers to increment by DMA_TRANSFER_SIZE after each transfer (or not)
    channel_config_set_read_increment(&config_1, true);
	channel_config_set_write_increment(&config_1, false);

    // Set pacing using given DREQ (data request)
    channel_config_set_dreq(&config_1, DREQ_SPI1_TX);

    dma_channel_configure(
        channel_1,
        &config_1,
        &spi_get_hw(spi1)->dr,
        src,
        size,
        false
    );
    
    dma_start_channel_mask(1u << channel_1);

    dma_channel_wait_for_finish_blocking(channel_1);
    dma_unclaim_mask(1u << channel_1);
}
