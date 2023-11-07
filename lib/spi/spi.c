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
#include <pico/time.h>
#include <dma.h>

void spi_device_init(spi_device_t *device){

	// Initialize SPI pins
    gpio_set_function(device->miso_gpio, GPIO_FUNC_SPI);
    gpio_set_function(device->mosi_gpio, GPIO_FUNC_SPI);
    gpio_set_function(device->sck_gpio, GPIO_FUNC_SPI);

    gpio_init(device->cs_gpio);
	gpio_set_dir(device->cs_gpio, GPIO_OUT);
	gpio_put(device->cs_gpio, 1);

    device->baudrate = spi_init(spi0, device->baudrate);
    // this might just need to just be the max baudrate of all the devices,
    // also need to find a way to determine spi0 or spi1, could also just move
	// it out of this function
}

void spi_write_read(spi_device_t *device, uint8_t *src, uint8_t *dst, size_t size){

		gpio_put(device->cs_gpio, 0);

		dmatransfer(src, &spi_get_hw(device->spi_inst)->dr, dst, size, spi_get_dreq(device->spi_inst, true), spi_get_dreq(device->spi_inst, false));

		while(spi_is_busy(device->spi_inst));

		gpio_put(device->cs_gpio, 1);
}

void spi_write(spi_device_t *device, uint32_t *src, size_t size){

		gpio_put(device->cs_gpio, 0);

		dmacpy(src, &spi_get_hw(device->spi_inst)->dr, size);

		while(spi_is_busy(device->spi_inst));

		gpio_put(device->cs_gpio, 1);
}
