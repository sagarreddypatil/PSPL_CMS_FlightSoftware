#include "emu.h"

#include <pico/unique_id.h>
#include <stdio.h>
#include <commandnet.h>
#include <sensornet.h>
#include <psp.h>

uint64_t time_offset;

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

  // --- Initialize Ethernet, it's the most important ---

  spi_init_bus_adv(spi1, 26, 27, 28, GPIO_SLEW_RATE_FAST,
                   GPIO_DRIVE_STRENGTH_4MA);

  w5500_set(w5500);
  w5500_reset(w5500);

  pico_unique_board_id_t board_id;
  pico_get_unique_board_id(&board_id);

  mac_t src_mac = {0x02,           board_id.id[3], board_id.id[4],
                   board_id.id[5], board_id.id[6], board_id.id[7]};

  while (!w5500_ready(w5500))
    tight_loop_contents();  // TODO: LEDs if we're stuck in this state

  /// --- Initialize all other devices ---
  spi_init_bus_adv(spi0, 2, 3, 4, GPIO_SLEW_RATE_FAST, GPIO_DRIVE_STRENGTH_4MA);

  max31856_set(tc_0);
  max31856_set(tc_1);
  ads13x_set(adc_0);
  w25n01_set(flash);

  // TODO verify all these devices function before startup

  // --- Ethernet ---
  w5500_set(w5500);
  w5500_config(w5500, src_mac, src_ip, subnet_mask, gateway);
  ip_t ip;
  w5500_read(w5500, W5500_COMMON, W5500_SIPR0, ip, sizeof(ip));
  printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", src_mac[0], src_mac[1],
         src_mac[2], src_mac[3], src_mac[4], src_mac[5]);
  printf("IP: %d.%d.%d.%d\n", src_ip[0], src_ip[1], src_ip[2], src_ip[3]);

  //------------Poll Initialization Complete------------
  while (!stdio_usb_connected())
    tight_loop_contents();  // TODO remove before flight

  //------------Main Program Begin------------

  for (int i = 0; i < 10; i++) printf("\n");
  printf(psp_logo);
  printf("Program: %s\n", PICO_PROGRAM_NAME);
  printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
  printf("\n=====Startup Information=====\n");
  printf("Took %llu µs\n", time_us_64());

  ads13x_set(adc_0);
  ads13x_reset(adc_0);
  while (!ads13x_ready(adc_0)) tight_loop_contents();
  ads13x_init(adc_0);

  stdio_flush();

  const uint64_t ADC_SAMPLE_PERIOD = 10;  // 100 hz
  uint64_t adcLastSampleTime       = 0;

  while (true)
    ;

  while (true) {
    const uint64_t now = time_us_64();
    if (now - adcLastSampleTime > ADC_SAMPLE_PERIOD) {
      uint16_t status;
      int32_t data[6];
      ads13x_read_data(adc_0, &status, data, 6);

      printf("time:\t%lld\tstatus:%x\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t\n", now,
             status, data[0], data[1], data[2], data[3], data[4], data[5]);

      adcLastSampleTime = now;
    }
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