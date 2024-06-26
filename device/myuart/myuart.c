#include <stdio.h>
#include <hardware/uart.h>
#include <uart.h>

void uart_device_init(uart_device_t *device) {
    gpio_set_function(device->rx_gpio, GPIO_FUNC_UART);
    gpio_set_function(device->tx_gpio, GPIO_FUNC_UART);

    uart_set_fifo_enabled(device->uart_inst, true);

    device->baudrate = uart_init(device->uart_inst, device->baudrate);
}

void uart_transfer(uart_device_t *device, volatile void *src, io_rw_32 *dr,
                   volatile void *dst, size_t size, uint tx_dreq,
                   uint rx_dreq) {
}