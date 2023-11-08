/* Kernel includes. */
#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */
#include "semphr.h"   /* Semaphore related API prototypes. */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "spi.h"
#include <w5500.h>

void setupHardware(){

}

#define STACK_SIZE 4096

StackType_t echoTaskStack[STACK_SIZE];
StaticTask_t echoTaskBuffer;


int main(){
    
    // xTaskCreateStatic(echo_main, "taskECHO", 512, NULL, 1, echoTaskStack, &echoTaskBuffer);

    vTaskStartScheduler();
    multicore_launch_core1(other core function);

    // should not reach here
    while(1);

    return 1;
}