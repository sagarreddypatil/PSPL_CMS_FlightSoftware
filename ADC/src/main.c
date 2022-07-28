// Big Cameron Was Here
#include "adc.h"
#include "uart.h"
#include "timer.h"

int main() {
    uart_init(2000000);
    uart_printf("Hello");
}