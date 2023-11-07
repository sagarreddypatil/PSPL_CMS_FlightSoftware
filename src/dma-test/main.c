#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <dma.h>
#include <math.h>

#define MAX_SIZE 8192

#define NUM_TRANSFERS 2

int main()
{   
    // Initalizes UART/USB output (USB currently, defined in CMakeLists)
    stdio_init_all();
    while(!stdio_usb_connected());


    uint8_t source1[MAX_SIZE];
    uint8_t source2[MAX_SIZE];
    uint8_t destination1[MAX_SIZE];
    uint8_t destination2[MAX_SIZE];

    // Filling source array with test data
    srand(time_us_64());
    for(size_t i = 0; i < MAX_SIZE; i++)
    {
        source1[i] = rand() % 256;
        source2[i] = rand() % 256;
    }
#if NUM_TRANSFERS == 1
    printf("Transfer Size   Time   Validity\n");
    printf("-------------------------------\n");
    for(size_t i = 0; i < log2(MAX_SIZE/8)+1; i++)
    {
        uint64_t start_time = time_us_64();
        // Initiate transfer 
        dmacpy(source1, destination1, 8*(1<<i));
        uint64_t end_time = time_us_64();


        // Confirms array transferred correctly
        bool validity = 1;
        for(size_t j = 0; j < 8*(1<<i); j++)
        {
            if(source1[j] != destination1[j])
            {
                validity = 0;
            }
        }

        // Prints success of transfer
        printf("  %5d bytes  %4lldus  %7s\n", 8*(1<<i), end_time-start_time, validity ? "Valid" : "Invalid");
    }
#endif

#if NUM_TRANSFERS == 2
    printf("  2x Transfer:   Validity    Time\n");
    printf("-------------------------------\n");
    for(size_t i = 0; i < log2(MAX_SIZE/8)+1; i++)
    {
        uint64_t start_time = time_us_64();
        // Initiate transfer 
        dmacpy2(source1, source2, destination1, destination2, 8*(1<<i));
        uint64_t end_time = time_us_64();


        // Confirms array transferred correctly
        bool validity = 1;
        for(size_t j = 0; j < 8*(1<<i); j++)
        {
            if(source1[j] != destination1[j] && source2[j] != destination2[j])
            {
                validity = 0;
            }
        }

        // Prints success of transfer
        printf("                 %s    %4lldus \n", validity ? "Valid" : "Invalid", end_time-start_time);
    }
#endif


    // To keep keep stdout alive and readable
    while(1);
    return 1;
}
