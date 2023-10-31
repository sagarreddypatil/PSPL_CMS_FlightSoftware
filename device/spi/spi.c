// Kernal Includes
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

// RP2040 Includes
#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <hardware/dma.h>
#include <hardware/irq.h>
#include <hardware/regs/dreq.h>

#include <spi.h>
#include <stdio.h>

void spi_device_init(spi_device_t *device){

	// Initialize SPI pins
    gpio_set_function(device->miso_gpio, GPIO_FUNC_SPI);
    gpio_set_function(device->mosi_gpio, GPIO_FUNC_SPI);
    gpio_set_function(device->sck_gpio, GPIO_FUNC_SPI);
    gpio_init(device->cs_gpio);
	gpio_set_dir(device->cs_gpio, GPIO_OUT);
	gpio_put(device->cs_gpio, 1);

	// Open and configure DMA channel to SPI TX FIFO
    device->tx_dma = dma_claim_unused_channel(true);
	device->tx_dma_config = dma_channel_get_default_config(device->tx_dma);
	channel_config_set_transfer_data_size(&device->tx_dma_config, DMA_TRANSFER_SIZE);
	channel_config_set_dreq(&device->tx_dma_config, spi_get_dreq(device->spi_inst, true));
	dma_channel_configure(
		device->tx_dma,
		&device->tx_dma_config,
		&spi_get_hw(device->spi_inst)->dr,
		NULL,
		1,
		false 
	);

	if (device->tx_dma == -1)
	{
		panic("NO DMA CHAN AVAIL!!!");
	}

	// Open and configure DMA channel to SPI RX FIFO
    device->rx_dma = dma_claim_unused_channel(true);
	device->rx_dma_config = dma_channel_get_default_config(device->rx_dma);
	channel_config_set_transfer_data_size(&device->rx_dma_config, DMA_TRANSFER_SIZE);
	channel_config_set_dreq(&device->rx_dma_config, spi_get_dreq(device->spi_inst, true));
	dma_channel_configure(
		device->rx_dma,
		&device->rx_dma_config,
		NULL,
		&spi_get_hw(device->spi_inst)->dr,
		1,
		false 
	);

	if (device->rx_dma == -1)
	{
		panic("NO DMA CHAN AVAIL!!!");
	}

    device->baudrate = spi_init(spi0, device->baudrate);
    // this might just need to just be the max baudrate of all the devices,
    // also need to find a way to determine spi0 or spi1, could also just move
	// it out of this function
	// device->task = xTaskGetCurrentTaskHandle();
}

void spi_write_read32(spi_device_t *device, uint32_t *src, uint32_t *dst, size_t size){

		channel_config_set_read_increment(&device->tx_dma_config, true);
		channel_config_set_write_increment(&device->tx_dma_config, false);

		channel_config_set_read_increment(&device->rx_dma_config, false);
		channel_config_set_write_increment(&device->rx_dma_config, true);

		dma_channel_set_read_addr(device->tx_dma, src, true);
		dma_channel_set_write_addr(device->rx_dma, dst, true);

		gpio_put(device->cs_gpio, 0);
		dma_start_channel_mask((1u << device->tx_dma) | (1u << device->rx_dma));
		dma_channel_wait_for_finish_blocking(device->tx_dma);
		gpio_put(device->cs_gpio, 1);
}

void spi_write_read16(spi_device_t *device, uint16_t *src, uint16_t *dst, size_t size){

		channel_config_set_read_increment(&device->tx_dma_config, true);
		channel_config_set_write_increment(&device->tx_dma_config, false);

		channel_config_set_read_increment(&device->rx_dma_config, false);
		channel_config_set_write_increment(&device->rx_dma_config, true);

		dma_channel_set_read_addr(device->tx_dma, src, true);
		dma_channel_set_write_addr(device->rx_dma, dst, true);

		gpio_put(device->cs_gpio, 0);
		dma_start_channel_mask((1u << device->tx_dma) | (1u << device->rx_dma));
		dma_channel_wait_for_finish_blocking(device->tx_dma);
		gpio_put(device->cs_gpio, 1);

}

void spi_write_read8(spi_device_t *device, uint8_t *src, uint8_t *dst, size_t size){

		channel_config_set_read_increment(&device->tx_dma_config, true);
		channel_config_set_write_increment(&device->tx_dma_config, false);

		channel_config_set_read_increment(&device->rx_dma_config, false);
		channel_config_set_write_increment(&device->rx_dma_config, true);

		dma_channel_set_read_addr(device->tx_dma, src, false);
		dma_channel_set_write_addr(device->rx_dma, dst, false);

		dma_channel_set_trans_count(device->tx_dma, size, false);
		dma_channel_set_trans_count(device->rx_dma, size, false);

		dma_channel_set_config(device->tx_dma, &device->tx_dma_config, false);
		dma_channel_set_config(device->rx_dma, &device->rx_dma_config, false);

		gpio_put(device->cs_gpio, 0);

		dma_start_channel_mask((1u << device->tx_dma) | (1u << device->rx_dma));

		dma_channel_wait_for_finish_blocking(device->rx_dma);
		if (dma_channel_is_busy(device->rx_dma)) {
       	 panic("RX completed before TX");
    	}
		gpio_put(device->cs_gpio, 1);
}

void spi_write32(spi_device_t *device, uint32_t *src, size_t size){

		channel_config_set_transfer_data_size(&device->tx_dma_config, DMA_TRANSFER_SIZE);

		// Makes DMA move it's read address by DMA_TRANSFER_SIZE after each transfer
		channel_config_set_read_increment(&device->tx_dma_config, true); //default value
		channel_config_set_write_increment(&device->tx_dma_config, false); // default value

		dma_channel_set_config(device->tx_dma, &device->tx_dma_config, false);
		
		// Sets number of DMA transfers to do
		dma_channel_set_trans_count(device->tx_dma, size, false);
		// Sets address to transfer from
		dma_channel_set_read_addr(device->tx_dma, src, false);

		gpio_put(device->cs_gpio, 0);

		dma_start_channel_mask(1u << device->tx_dma);
		dma_channel_wait_for_finish_blocking(device->tx_dma);

		gpio_put(device->cs_gpio, 1);
}

void spi_write8(spi_device_t *device, uint8_t *src, size_t size){

		channel_config_set_transfer_data_size(&device->tx_dma_config, DMA_TRANSFER_SIZE);

		// Makes DMA move it's read address by DMA_TRANSFER_SIZE after each transfer
		channel_config_set_read_increment(&device->tx_dma_config, true); //default value
		channel_config_set_write_increment(&device->tx_dma_config, false); // default value

		dma_channel_set_config(device->tx_dma, &device->tx_dma_config, false);
		
		// Sets number of DMA transfers to do
		dma_channel_set_trans_count(device->tx_dma, size, false);
		// Sets address to transfer from
		dma_channel_set_read_addr(device->tx_dma, src, false);

		gpio_put(device->cs_gpio, 0);

		dma_start_channel_mask(1u << device->tx_dma);
		dma_channel_wait_for_finish_blocking(device->tx_dma);

		gpio_put(device->cs_gpio, 1);
}

void dma_move(uint8_t *src, uint8_t *dst, size_t size)
{
	// Makes sure size is always number of transfers (is initially in bytes)
	switch (DMA_TRANSFER_SIZE)
	{
		case DMA_SIZE_8:
			size /= 1;
			break;
		case DMA_SIZE_16:
			size /= 2;
			break;
		case DMA_SIZE_32:
			size /= 4;
			break;
	}

	uint channel = dma_claim_unused_channel(true);
	dma_channel_config config = dma_channel_get_default_config(channel);

	channel_config_set_transfer_data_size(&config, DMA_TRANSFER_SIZE);
	dma_channel_set_trans_count(channel, size, false);

	dma_channel_set_read_addr(channel, src, false);
	dma_channel_set_write_addr(channel, dst, false);

	channel_config_set_read_increment(&config, true);
	channel_config_set_write_increment(&config, true);

	dma_channel_set_config(channel, &config, false);

	dma_channel_start(channel);

	dma_channel_wait_for_finish_blocking(channel);
}

void spi_irq_handler(spi_device_t *device) {
    // Clear the interrupt request.
    dma_hw->ints0 = 1u << device->rx_dma;

    /* The xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
     it will get set to pdTRUE inside the interrupt safe API function if a
     context switch is required. */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Send a notification directly to the task to which interrupt processing is
     being deferred. */
    vTaskNotifyGiveIndexedFromISR(
        device->task,  // The handle of the task to which the
                      // notification is being sent.
        1,  // uxIndexToNotify: The index within the target task's array of
            // notification values to which the notification is to be sent.
        &xHigherPriorityTaskWoken);

    /* Pass the xHigherPriorityTaskWoken value into portYIELD_FROM_ISR().
    If xHigherPriorityTaskWoken was set to pdTRUE inside
     vTaskNotifyGiveFromISR() then calling portYIELD_FROM_ISR() will
     request a context switch. If xHigherPriorityTaskWoken is still
     pdFALSE then calling portYIELD_FROM_ISR() will have no effect. */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	return;
}
