// RP2040 Includes
#include <hardware/gpio.h>
#include <hardware/uart.h>
#include <hardware/regs/dreq.h>

#include <spi.h>
#include <stdio.h>
#include <pico/time.h>
#include <dma.h>

typedef struct {
  uart_inst_t *uart_inst;
  const uint8_t rx_gpio;
  const uint8_t tx_gpio;

  uint baudrate;  // is not guaranteed, actual is set when device initialized
} uart_device_t;

/*! @brief initialize an UART device, writing configuration to provided struct
 *   @param device UART struct that will be configured
 */
void uart_device_init(uart_device_t *device);

void uart_transfer(uart_device_t *device, volatile void *src, io_rw_32 *dr,
                   volatile void *dst, size_t size, uint tx_dreq, uint rx_dreq);

void uart_read(uart_device_t *device);

void uart_write(uart_device_t *device);