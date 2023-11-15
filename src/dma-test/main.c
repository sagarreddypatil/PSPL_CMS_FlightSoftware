#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <dma.h>
#include <string.h>
#include <math.h>

#define MAX_SIZE   8192*2
#define MIN_SIZE   8192
#define NUM_EPOCHS 1

// Arrays must be of size log2(MAX_SIZE/MIN_SIZE)+1
void transfer_epoch(uint64_t *cpytimes, uint64_t *dma1times,
                    uint64_t *dma4times) {
  for (size_t i = MIN_SIZE+1; i < MAX_SIZE+1; i++) {
    uint8_t source[MAX_SIZE];
    uint8_t destination[MAX_SIZE];

    // Filling source array with test data
    srand(time_us_64());
    for (size_t j = 0; j < MAX_SIZE; j++) {
      source[j] = rand() % 256;
    }
    uint64_t start_time = time_us_64();
    dmacpy(source, destination, i);
    uint64_t end_time = time_us_64();

    uint64_t start_time2 = time_us_64();
    memcpy(destination, source, i);
    uint64_t end_time2 = time_us_64();

    uint64_t start_time3 = time_us_64();
    dmacpy4(source, destination, i);
    uint64_t end_time3 = time_us_64();

    cpytimes[i] += end_time2 - start_time2;
    dma1times[i] += end_time - start_time;
    dma4times[i] += end_time3 - start_time3;

    printf("%d,%lld,%lld,%lld\n",
           i, end_time - start_time, end_time3 - start_time3,
           end_time2 - start_time2);
  }
}

int main() {
  // Initalizes UART/USB output (USB currently, defined in CMakeLists)
  stdio_init_all();
  while (!stdio_usb_connected())
    ;

  uint64_t cpytimes[MAX_SIZE-MIN_SIZE]  = {0};
  uint64_t dma1times[MAX_SIZE-MIN_SIZE] = {0};
  uint64_t dma4times[MAX_SIZE-MIN_SIZE] = {0};

  for (size_t i = 0; i < NUM_EPOCHS; i++) {
    transfer_epoch(cpytimes, dma1times, dma4times);
  }

  // printf("Transfer Size   DMA (1 byte)    DMA (8 byte)    memcpy\n");
  // printf("------------------------------------------------------\n");
  // for (size_t i = 0; i < log2(MAX_SIZE / MIN_SIZE) + 1; i++) {
  //   printf(" %5d bytes      %4lldus          %4lldus        %4lldus\n",
  //          4 * (1 << i), dma1times[i] / NUM_EPOCHS, dma4times[i] / NUM_EPOCHS,
  //          cpytimes[i] / NUM_EPOCHS);
  // }

  // To keep keep stdout alive and readable
  while (1)
    ;
  return 1;
}
