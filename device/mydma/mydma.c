#include <mydma.h>

void dmacpy(volatile void *dst, const void *src, size_t size) {
    uint channel              = dma_claim_unused_channel(true);
    dma_channel_config config = dma_channel_get_default_config(channel);

    // Sets number of bits to move per transfer
    channel_config_set_transfer_data_size(&config, DMA_DEFAULT_TRANSFER_SIZE);

    // Sets read/write pointers to increment by DMA_DEFAULT_TRANSFER_SIZE after
    // each transfer
    channel_config_set_read_increment(&config, true);
    channel_config_set_write_increment(&config, true);

    dma_channel_configure(channel, &config, dst, src, size, true);

    dma_channel_wait_for_finish_blocking(channel);
    dma_unclaim_mask(1 << channel);
}
