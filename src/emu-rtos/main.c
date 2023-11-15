/* Kernel includes. */
#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */
#include "semphr.h"   /* Semaphore related API prototypes. */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include <spi.h>
#include <w5500.h>
#include <dma.h>

#include <emu.h>
#include <commandnet.h>
#include <sensornet.h>
#include <pico/unique_id.h>


#define STACK_SIZE 4096

void setupHardware()
{
    stdio_init_all();
  /*
   * below rates are default, fast slew not yet tested, need to scope
   * if signal ripples ripples, lower the rate
   * if doesn't rise/fall to correct voltages, raise the rate
   *
   * See:
   * Falstad Circuit Simulator
   * https://tinyurl.com/2m9dxu53
   */

  // --- Ethernet ---
  spi_init_bus_adv(spi0, 16, 18, 19, GPIO_SLEW_RATE_FAST,
                   GPIO_DRIVE_STRENGTH_4MA);

  w5500_set(w5500);
  w5500_reset(w5500);

  pico_unique_board_id_t board_id;
  pico_get_unique_board_id(&board_id);

  mac_t src_mac = {0x02,           board_id.id[3], board_id.id[4],
                   board_id.id[5], board_id.id[6], board_id.id[7]};

  /// --- ADC ---
  spi_init_bus(spi1, 11, 12, 14);

  ads13x_set(emu_adc);
  ads13x_reset(emu_adc);

  //------------Poll Initialization Complete------------
  while (!stdio_usb_connected()) tight_loop_contents();
  while (!w5500_ready(w5500)) tight_loop_contents();
  while (!ads13x_ready(emu_adc)) tight_loop_contents();

  for (int i = 0; i < 10; i++) printf("\n");
}


StackType_t echoTaskStack[STACK_SIZE];
StaticTask_t echoTaskBuffer;


int main(){
    
    xTaskCreateStatic(echo_main, "taskECHO", 512, NULL, 1, echoTaskStack, &echoTaskBuffer);

    vTaskStartScheduler();
    multicore_launch_core1(other core function);

    // should not reach here
    while(1);

    return 1;
}