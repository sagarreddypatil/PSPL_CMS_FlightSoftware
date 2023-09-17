#include "FreeRTOS.h"
#include "task.h"

#include "pico/multicore.h"

void vLaunch() {
}

void prvSetupHardware() {
}

int main(void) {
  prvSetupHardware();

  vLaunch();
  multicore_launch_core1(vLaunch);

  while (1) {
  }
}
