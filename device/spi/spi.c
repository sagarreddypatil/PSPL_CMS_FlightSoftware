// Kernal Includes
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

// RP2040 Includes
#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <hardware/dma.h>
#include <hardware/irq.h>

#include <spi.h>
#include <stdio.h>

void spi_device_init(spi_device_t *device){

	// Initialize SPI pins
    gpio_set_function(device->miso_gpio, GPIO_FUNC_SPI);
    gpio_set_function(device->mosi_gpio, GPIO_FUNC_SPI);
    gpio_set_function(device->sck_gpio, GPIO_FUNC_SPI);
    gpio_init(device->cs_gpio);

	// Open and configure DMA channel to SPI TX FIFO
    device->tx_dma = dma_claim_unused_channel(true);
	device->tx_dma_config = dma_channel_get_default_config(device->tx_dma);
	dma_channel_configure(
		device->tx_dma,
		&device->tx_dma_config,
		&spi_get_hw(spi_default)->dr,
		NULL,
		1,
		false 
	);
	channel_config_set_dreq(&device->tx_dma_config, DREQ_SPI0_TX);
	channel_config_set_transfer_data_size(&device->tx_dma_config, DMA_TRANSFER_SIZE);

	// Open and configure DMA channel to SPI RX FIFO
    device->rx_dma = dma_claim_unused_channel(true);
	device->rx_dma_config = dma_channel_get_default_config(device->rx_dma);
	dma_channel_configure(
		device->rx_dma,
		&device->rx_dma_config,
		NULL,
		&spi_get_hw(spi_default)->dr,
		1,
		false 
	);
	channel_config_set_dreq(&device->rx_dma_config, DREQ_SPI0_RX);
	channel_config_set_transfer_data_size(&device->rx_dma_config, DMA_TRANSFER_SIZE);

    device->baudrate = spi_init(spi0, device->baudrate);
    // this might just need to just be the max baudrate of all the devices,
    // also need to find a way to determine spi0 or spi1 
	device->task = xTaskGetCurrentTaskHandle();
}

void spi_write_read32(spi_device_t *device, uint32_t *src, uint32_t *dst, size_t size){

	if (spi_is_writable(device->spi_inst) && spi_is_readable(device->spi_inst)){

		BaseType_t rc = ulTaskNotifyTake(pdTRUE, 0);

		channel_config_set_read_increment(&device->tx_dma_config, true);
		channel_config_set_write_increment(&device->rx_dma_config, true);

		dma_channel_set_read_addr(device->tx_dma, dst, true);
		dma_channel_set_write_addr(device->rx_dma, src, true);

		dma_channel_set_trans_count(device->tx_dma, size/8 + 1, false);
		dma_channel_set_trans_count(device->rx_dma, size/8 + 1, false);

		dma_start_channel_mask((1u << device->tx_dma) | (1u << device->rx_dma));

		/* Timeout 1 sec */
		uint32_t timeOut = 1000;
		/* Wait until master completes transfer or time out has occured. */
		rc = ulTaskNotifyTakeIndexed(
        1, pdFALSE, pdMS_TO_TICKS(timeOut));  // Wait for notification from ISR

		if (!rc) {
			// This indicates that xTaskNotifyWait() returned without the
			// calling task receiving a task notification. The calling task will
			// have been held in the Blocked state to wait for its notification
			// state to become pending, but the specified block time expired
			// before that happened.
			printf("Task %s timed out!!\n",
					pcTaskGetName(xTaskGetCurrentTaskHandle()));
			return;
   		}

	}
}

void spi_write_read16(spi_device_t *device, uint16_t *src, uint16_t *dst, size_t size){

	if (spi_is_writable(device->spi_inst) && spi_is_readable(device->spi_inst)){

		BaseType_t rc = ulTaskNotifyTake(pdTRUE, 0);

		channel_config_set_read_increment(&device->tx_dma_config, true);
		channel_config_set_write_increment(&device->rx_dma_config, true);

		dma_channel_set_read_addr(device->tx_dma, dst, true);
		dma_channel_set_write_addr(device->rx_dma, src, true);

		dma_channel_set_trans_count(device->tx_dma, size/8 + 1, false);
		dma_channel_set_trans_count(device->rx_dma, size/8 + 1, false);

		dma_start_channel_mask((1u << device->tx_dma) | (1u << device->rx_dma));

		/* Timeout 1 sec */
		uint32_t timeOut = 1000;
		/* Wait until master completes transfer or time out has occured. */
		rc = ulTaskNotifyTakeIndexed(
        1, pdFALSE, pdMS_TO_TICKS(timeOut));  // Wait for notification from ISR

		if (!rc) {
			// This indicates that xTaskNotifyWait() returned without the
			// calling task receiving a task notification. The calling task will
			// have been held in the Blocked state to wait for its notification
			// state to become pending, but the specified block time expired
			// before that happened.
			printf("Task %s timed out!!\n",
					pcTaskGetName(xTaskGetCurrentTaskHandle()));
			return;
   		}

	}
}

void spi_write_read8(spi_device_t *device, uint8_t *src, uint8_t *dst, size_t size){

	if (spi_is_writable(device->spi_inst) && spi_is_readable(device->spi_inst)){

		BaseType_t rc = ulTaskNotifyTake(pdTRUE, 0);

		channel_config_set_read_increment(&device->tx_dma_config, true);
		channel_config_set_write_increment(&device->rx_dma_config, true);

		dma_channel_set_read_addr(device->tx_dma, dst, true);
		dma_channel_set_write_addr(device->rx_dma, src, true);

		dma_channel_set_trans_count(device->tx_dma, size/8 + 1, false);
		dma_channel_set_trans_count(device->rx_dma, size/8 + 1, false);

		dma_start_channel_mask((1u << device->tx_dma) | (1u << device->rx_dma));

		/* Timeout 1 sec */
		uint32_t timeOut = 1000;
		/* Wait until master completes transfer or time out has occured. */
		rc = ulTaskNotifyTakeIndexed(
        1, pdFALSE, pdMS_TO_TICKS(timeOut));  // Wait for notification from ISR

		if (!rc) {
			// This indicates that xTaskNotifyWait() returned without the
			// calling task receiving a task notification. The calling task will
			// have been held in the Blocked state to wait for its notification
			// state to become pending, but the specified block time expired
			// before that happened.
			printf("Task %s timed out!!\n",
					pcTaskGetName(xTaskGetCurrentTaskHandle()));
			return;
   		}

	}
}

void spi_write(spi_device_t *device, uint32_t *src, size_t size){

	if (spi_is_writable(device->spi_inst)){
		// Makes DMA move it's read address by DMA_TRANSFER_SIZE after each transfer
		channel_config_set_read_increment(&device->tx_dma_config, true);
		
		// Sets number of DMA transfers to do
		dma_channel_set_trans_count(device->tx_dma, size/8+1, false);
		// Sets address to transfer from
		dma_channel_set_read_addr(device->tx_dma, src, true);

		dma_channel_wait_for_finish_blocking(device->tx_dma); // @todo Timeout needed

	}
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
