// FreeRTOS Includes
#include <FreeRTOS.h>
#include <task.h>

// RP2040 Includes
#include <hardware/gpio.h>
#include <hardware/irq.h>
#include <emu.h>

#include <stdio.h>
#include <pico/mutex.h>


// Needs to move w5500 data to queue?
void w5500_drdy()
{

}

void w5500_drdy_handler()
{
    portDISABLE_INTERRUPTS();
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // vTaskNotifyGiveFromISR(TASK, &xHigherPriorityTaskWoken);


    portENABLE_INTERRUPTS();
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}