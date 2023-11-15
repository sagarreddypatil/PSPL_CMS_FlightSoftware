#include <stdio.h>
#include <hardware/uart.h>

struct uart_device_t {
    
}

/*! @brief initialize an UART device, writing configuration to provided struct
*   @param device UART struct that will be configured
*/
void uart_device_init(uart_device_t *device);