#include <pico/stdlib.h>
#include <stdio.h>
 
int main()
{
    const uint OX_PIN = 10;
    const uint FU_PIN = 9;
    const uint AUX_PIN = 15;
 
    // Initialize the GPIO to blink the LED
    //
    gpio_init(OX_PIN);
    gpio_set_dir(OX_PIN, GPIO_OUT);
    gpio_init(AUX_PIN);
    gpio_set_dir(AUX_PIN, GPIO_OUT);
    gpio_init(FU_PIN);
    gpio_set_dir(FU_PIN, GPIO_OUT);
 
    // Initialize the serial port.
    //
    stdio_init_all();
 
    // Loop to blink the LED and print the message.
    //
    while (true)
    {
        // printf("Wax on!\n");
           gpio_put(OX_PIN, 0);
           gpio_put(FU_PIN, 0);
           sleep_ms(1000);
    
            // printf("Wax off!\n");
        //     gpio_put(OX_PIN, 0);
        //     gpio_put(FU_PIN, 0);
        //     sleep_ms(1000);
    }
 
    return 0;
}
