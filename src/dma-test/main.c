#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <dma.h>
#include <math.h>

#define MAX_SIZE 65536

int main()
{   
    // Initalizes UART/USB output (USB currently, defined in CMakeLists)
    stdio_init_all();
    while(!stdio_usb_connected());


    uint8_t source[MAX_SIZE];
    uint8_t destination[MAX_SIZE];

    // Filling source array with test data
    srand(time_us_64());
    for(size_t i = 0; i < MAX_SIZE; i++)
    {
        source[i] = rand() % 256;
    }

    printf("Transfer Size   Time   Validity\n");
    printf("-------------------------------\n");
    for(size_t i = 0; i < log2(MAX_SIZE/8)+1; i++)
    {
        uint64_t start_time = time_us_64();
        // Initiate transfer 
        dmacpy(source, destination, 8*(1<<i));
        uint64_t end_time = time_us_64();


        // Confirms array transferred correctly
        bool validity = 1;
        for(size_t j = 0; j < 8*(1<<i); j++)
        {
            if(source[j] != destination[j])
            {
                validity = 0;
            }
        }

        // Prints success of transfer
        printf("  %5d bytes  %4lldus  %7s\n", 8*(1<<i), end_time-start_time, validity ? "Valid" : "Invalid");
    }
    

    // To keep keep stdout alive and readable
    while(1);
    return 1;
}
