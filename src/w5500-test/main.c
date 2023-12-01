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
#include <stdarg.h>

#include "psp.h"

// void sprintf(const char* format, ...);

spi_bus_t bus_1 = {
  .bus = spi1,
  .index = 1,
};

spi_device_t w5500 = { //posi pico sclk
  .spi_bus = &bus_1,
  .cs_gpio = 25,
  .baudrate = 30000000,
  };

void not_main() {
  stdio_flush();

  for(int i = 0; i < 12; i++)
  {
    printf("\n");
  }

  printf(psp_logo);
  printf("Program: %s\n", PICO_PROGRAM_NAME);
  printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);

  ip_t gateway     = {192, 168, 2, 1};
  ip_t subnet_mask = {255, 255, 255, 0};
  ip_t src_ip      = {192, 168, 2, 50};
  mac_t src_mac    = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};

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
  printf("Free Space: %d bytes\n", space);

  // print ip
  ip_t ip;
  w5500_read(&w5500, W5500_COMMON, W5500_SIPR0, ip, sizeof(ip));
  printf("Connected, IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);


  while (true)
  {
    uint8_t phyreg = w5500_read8(&w5500, W5500_COMMON, W5500_PHYCFGR);
    printf("0x%x\n", phyreg);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void not_not_not_main()
{
  w5500_reset(&w5500);
  uint64_t start = time_us_64();
  while (!w5500_ready(&w5500))
    ;
  printf("W5500 ready, awaiting link, took %d us\n",
         (int)(time_us_64() - start));

  while (!w5500_has_link(&w5500))
    ;
  printf("W5500 has link, took %d us\n", (int)(time_us_64() - start));

  ip_t gateway     = {192, 168, 2, 1};
  ip_t subnet_mask = {255, 255, 255, 0};
  ip_t src_ip      = {192, 168, 2, 50};
  mac_t src_mac    = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};

  w5500_config(&w5500, src_mac, src_ip, subnet_mask, gateway);
  ip_t victim          = {192, 168, 2, 1};
  uint16_t victim_port = 5000;
  w5500_create_udp_socket(&w5500, W5500_S0, 5000, false, false, false);

  char msg[1024];
  for (int i = 0; i < 1024; i++) {
    msg[i] = 'A';
  }

  w5500_write(&w5500, W5500_S0, W5500_Sn_DIPR0, victim, 4);
  w5500_write16(&w5500, W5500_S0, W5500_Sn_DPORT0, victim_port);

  while (true) {
    // blast UDP packets to victim
    w5500_write_data(&w5500, W5500_S0, msg, sizeof(msg));
  }
}


void not_not_main()
{
  while(true)
  {
    uint16_t space = w5500_read16(&w5500, W5500_S0, W5500_Sn_TX_FSR0);
    printf("Free Space: %d bytes\n", space);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

#define PROC_STACK_SIZE 2048

int main()
{
  stdio_init_all();
  while (!stdio_usb_connected())
    ; // @todo timeout needed
  
  spi_device_init(&w5500, 27, 26, 28);

  StaticTask_t task;
  StaticTask_t task2;
  StaticTask_t task3;
  StackType_t buffer[PROC_STACK_SIZE];
  StackType_t buffer2[PROC_STACK_SIZE];
  StackType_t buffer3[PROC_STACK_SIZE];

  xTaskCreateStatic(not_main, "w5500_main", PROC_STACK_SIZE, NULL, 1, buffer, &task);
  xTaskCreateStatic(not_not_main, "random", PROC_STACK_SIZE, NULL, 1, buffer2, &task2);
  xTaskCreateStatic(not_not_not_main, "lotsofpackets", PROC_STACK_SIZE, NULL, 1, buffer3, &task3);

  StaticSemaphore_t mutex_buf;
  SemaphoreHandle_t mutex;

  // StaticSemaphore_t print_mutex_buffer;
  // SemaphoreHandle_t print_mutex = NULL;

  // print_mutex = xSemaphoreCreateMutexStatic(&print_mutex_buffer);
  mutex = xSemaphoreCreateMutexStatic(&mutex_buf);

  w5500.spi_bus->mutex = mutex;

  vTaskStartScheduler();

  while(1);

  return 0;
}

// void sprint(const char* format, ...)
// { 
//     xSemaphoreTake(print_mutex, portMAX_DELAY);

//     va_list args;
//     va_start(args, format);
//     vprintf(format, args);
//     va_end(args);
    
//     xSemaphoreGive(print_mutex);
// }
