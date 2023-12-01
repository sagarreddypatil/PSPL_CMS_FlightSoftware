#include <w5500.h>
#include <w5500/tcp_server.h>
#include <myspi.h>
#include <pico/binary_info.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <mydma.h>
#include <semphr.h>

#include "psp.h"

void sampleSharedResouceFunction(char* out);
void vTaskSMP_print_core();

spi_bus_t bus_1 = {
  .bus = spi1,
};

spi_device_t w5500 = { //posi pico sclk
  .spi_bus = &bus_1,
  .cs_gpio = 25,
  .baudrate = 30000000
  };

void not_main() {
  for(int i = 0; i < 12; i++)
  {
    printf("\n");
    // dma_channel_unclaim(i);
  }

  printf(psp_logo);
  printf("Program: %s\n", PICO_PROGRAM_NAME);
  printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);

  printf("started spinning\n");

  for(int i = 0; i < 1000000; i++);

  printf("finished spinning\n");

  ip_t gateway     = {192, 168, 2, 1};
  ip_t subnet_mask = {255, 255, 255, 0};
  ip_t src_ip      = {192, 168, 2, 50};
  mac_t src_mac    = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};

  printf("bug12312");
  w5500_reset(&w5500);
  uint64_t start = time_us_64();

  uint count = 0;
  do {
    count++;
  } while (!w5500_ready(&w5500));
  printf("W5500 ready, took %d us after %d tries\n", (int)(time_us_64() - start), count);

  count = 0;
  do {
    count++;
    sleep_ms(1750);
  } while (!w5500_has_link(&w5500));
  printf("W5500 has link, took %d us after %d tries\n", (int)(time_us_64() - start), count);

  w5500_config(&w5500, src_mac, src_ip, subnet_mask, gateway);

  uint16_t space = w5500_read16(&w5500, W5500_S3, W5500_Sn_TX_FSR0);
  printf("Free Space: %d bytes", space);

  // print ip
  ip_t ip;
  w5500_read(&w5500, W5500_COMMON, W5500_SIPR0, ip, sizeof(ip));
  printf("Connected, IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);


  while (true){
    uint8_t phyreg = w5500_read8(&w5500, W5500_COMMON, W5500_PHYCFGR);
    printf("0x%x\n", phyreg);
    // sleep_ms(500);
  }
}

#define PROC_STACK_SIZE 1024

StaticTask_t task;
StackType_t buffer[PROC_STACK_SIZE];

int main()
{
  stdio_init_all();
  while (!stdio_usb_connected())
    ; // @todo timeout needed
  
  spi_device_init(&w5500, 27, 26, 28);

  xTaskCreateStatic(not_main, "w5500_main", PROC_STACK_SIZE, NULL, 4, buffer, &task);
  irq_add_shared_handler(DMA_IRQ_0, dma_finished_isr0, 1);
  irq_add_shared_handler(DMA_IRQ_1, dma_finished_isr1, 1);
  // irq_set_enabled(DMA_IRQ_0, true);
  // irq_set_enabled(DMA_IRQ_1, true);

  vTaskStartScheduler();

  while(1);

  return 0;
}

StaticSemaphore_t print_mutex_buffer;
SemaphoreHandle_t print_mutex = NULL;

void sprint(char* out)
{   
    xSemaphoreTake(print_mutex, portMAX_DELAY);

    puts(out); 
    
    xSemaphoreGive(print_mutex);

}

void vTaskSMP_print_core()
{
    TaskHandle_t handle = xTaskGetCurrentTaskHandle();

    UBaseType_t mask = vTaskCoreAffinityGet(handle);

    char *name = pcTaskGetName(handle);

    char out[32];

    for (;;) {

        sprintf(out,"%s  %d  %ld\n ", name, get_core_num(), xTaskGetTickCount());

        sprint(out);

        vTaskDelay(100);
    }
}
