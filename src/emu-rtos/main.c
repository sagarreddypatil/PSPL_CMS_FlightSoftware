/* Kernel includes. */
#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */
#include "semphr.h"   /* Semaphore related API prototypes. */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include <myspi.h>
#include <w5500.h>
#include <mydma.h>

#include <emu.h>
#include <commandnet.h>
#include <sensornet.h>
#include <pico/unique_id.h>

#define STACK_SIZE 4096

void setupHardware() {
  stdio_usb_init();

  /*
   * below rates are default, fast slew not yet tested, need to scope
   * if signal ripples ripples, lower the rate
   * if doesn't rise/fall to correct voltages, raise the rate
   *
   * See:
   * Falstad Circuit Simulator
   * https://tinyurl.com/2m9dxu53
   */

  // --- SPI --- //
  StaticSemaphore_t spi0_mutex_buffer;
  myspi_bus_init(myspi0, &spi0_mutex_buffer);

  StaticSemaphore_t spi1_mutex_buffer;
  myspi_bus_init(myspi1, &spi1_mutex_buffer);

  // --- Ethernet --- //
  myspi_device_t w5500;

  myspi_device_init(&w5500, myspi1, 25U, 27U, 26U, 28U, 1, 1, 30000000);

  pico_unique_board_id_t board_id;
  pico_get_unique_board_id(&board_id);

  mac_t src_mac = {0x02,           board_id.id[3], board_id.id[4],
                   board_id.id[5], board_id.id[6], board_id.id[7]};

  // --- ADC --- //
  myspi_device_t ads13x;
  // myspi_device_init(&ads13x, );

  //------------Poll Initialization Complete------------
  while (!stdio_usb_connected()) tight_loop_contents();
  while (!w5500_ready(&w5500)) tight_loop_contents();
  while (!ads13x_ready(&ads13x)) tight_loop_contents();

  for (int i = 0; i < 10; i++) printf("\n");
}

StackType_t w5500_task_stack[128];
StaticTask_t w5500_drdy_task_buffer;

int main() {
  setupHardware();

  //
  TaskHandle_t w5500_drdy_task = xTaskCreateStaticAffinitySet(
      w5500_drdy_handler, "W5500_DRDY", 128, NULL, 1, w5500_task_stack,
      &w5500_drdy_task_buffer, 1 << (0));

  vTaskStartScheduler();

  // should not reach here
  while (true)
    ;

  return EXIT_FAILURE;
}