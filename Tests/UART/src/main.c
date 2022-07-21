#include "uart.h"
#include <util/delay.h>

int main() {
    uart_init(2000000);
	char byte;

	uart_transmit('\n');
	uart_printf("Hello from UART!");
	while(1 == 1) {
		byte = uart_receive();
		uart_printf("You typed: %c\n", byte);
	}

    return 0;
}