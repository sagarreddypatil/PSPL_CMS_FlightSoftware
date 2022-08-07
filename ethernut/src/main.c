#include "ethernut.h"
#include "uart.h"
#include "timer.h"

int main() {
    uart_init(2000000);
    spi_init(SPI_MSB, SPI_MODE0, SPI_SPEED_PLAID);
    uint32_t ipv4 = 0xBCBCBC01;
    spi_slave_t eth = ethernut_init(&DDRB, &PORTB, PINB0, ipv4);

    uint64_t finish;

    while(1) {
        timer_init();
        ethernut_frame(eth, ETH_MR, ETH_SOCK_0_REG, ETH_READ, NULL, 0);
        finish = timer_micros();
        uart_printf("%lduS\n", finish);
    }
}