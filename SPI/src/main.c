/*
Built by Mr. Dr. Lord Master God Emperor Supreme Chancelor Overlord etc etc etc Big Cameron himself

Here Big Cameron from the planet Earth first started coding CMS, July 10th, 2022

We came in hate for Blue Origin

Starting this project was kinda like this: https://youtu.be/vFUx_KC1bHQ?t=77
*/

#include "spi.h"

int main() {
    init_spi();
    uint8_t data;

    while(1 == 1) {
        command_spi(0b10101010);
        sleep_ms(500);
        data = read_spi();
        sleep_ms(500);
        print_byte(data);
    }
}