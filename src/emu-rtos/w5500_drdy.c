// glibc Includes
#include <stdio.h>

// FreeRTOS Includes
#include <FreeRTOS.h>
#include <task.h>

// Pico Includes
// ------------- //

// Library Includes
// ---------------- //

// Device Includes
#include <w5500.h>

// EMU Includes
#include <emu.h>

// Needs to move w5500 data to queue?
void w5500_drdy() {
  uint32_t ulNotificationValue;
  ulNotificationValue = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(portMAX_DELAY));

  assert(ulNotificationValue);
  ulNotificationValue = 0;


}

void w5500_drdy_handler() {
  portDISABLE_INTERRUPTS();
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  vTaskNotifyGiveFromISR(w5500_drdy_task, &xHigherPriorityTaskWoken);

  portENABLE_INTERRUPTS();
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}