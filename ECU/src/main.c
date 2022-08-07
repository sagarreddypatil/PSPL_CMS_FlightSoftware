// Big Cameron Was Here

#include "ecu.h"
#include "uart.h"
#include "timer.h"

int main() {
    uart_init(2000000); 
    uart_printf("Size of data_t: %d\n", sizeof(ecu_data_t));
    uint64_t finish;
    ecu_data_t data;

    data.pressure_1 = 0xFFFF;
    data.pressure_2 = 0xFFFF;
    data.pressure_3 = 0xFFFF;
}