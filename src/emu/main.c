#include "emu.h"

#include <pico/unique_id.h>
#include <stdio.h>
#include <commandnet.h>
#include <sensornet.h>

int main() {
  //------------All Initialization------------
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

  //------------Main Program Begin------------

  printf("Program: %s\n", PICO_PROGRAM_NAME);
  printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
  printf("\n=====Startup Information=====\n");
  printf("Took %llu µs\n", time_us_64());

  //------------Setup and Configuration------------

  // --- Ethernet ---
  w5500_config(w5500, src_mac, src_ip, subnet_mask, gateway);
  ip_t ip;
  w5500_read(w5500, W5500_COMMON, W5500_SIPR0, ip, sizeof(ip));
  printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", src_mac[0], src_mac[1],
         src_mac[2], src_mac[3], src_mac[4], src_mac[5]);
  printf("IP: %d.%d.%d.%d\n", src_ip[0], src_ip[1], src_ip[2], src_ip[3]);

  // --- ADC ---
  ads13x_init(emu_adc);

  cmdnet_task_init();
  sensornet_task_init();
  // solenoid_task_init();

  gpio_init(PYRO);
  gpio_put(PYRO, false);
  gpio_set_dir(PYRO, true);

  while (true) {
    cmdnet_task_run();
    sensornet_task_run();
    // solenoid_task_run();

    gpio_put(PYRO, pyro_state);
  }
}

void vApplicationTickHook(void) {
#if mainCREATE_SIMPLE_BLINKY_DEMO_ONLY == 0
  {
/* The full demo includes a software timer demo/test that requires
prodding periodically from the tick interrupt. */
#if (mainENABLE_TIMER_DEMO == 1)
    vTimerPeriodicISRTests();
#endif

/* Call the periodic queue overwrite from ISR demo. */
#if (mainENABLE_QUEUE_OVERWRITE == 1)
    vQueueOverwritePeriodicISRDemo();
#endif

/* Call the periodic event group from ISR demo. */
#if (mainENABLE_EVENT_GROUP == 1)
    vPeriodicEventGroupsProcessing();
#endif

/* Call the code that uses a mutex from an ISR. */
#if (mainENABLE_INTERRUPT_SEMAPHORE == 1)
    vInterruptSemaphorePeriodicTest();
#endif

/* Call the code that 'gives' a task notification from an ISR. */
#if (mainENABLE_TASK_NOTIFY == 1)
    xNotifyTaskFromISR();
#endif
  }
#endif
}