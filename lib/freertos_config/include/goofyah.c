#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>

void vApplicationTickHook( void ) {
    
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName ) {
    for (size_t i = 0; i < 100; i++)
    {
        printf("STACK OVERFLOW");
    }
    
}