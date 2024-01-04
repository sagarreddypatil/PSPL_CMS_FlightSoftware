// Standard Includes
#include <stdio.h>

// FreeRTOS Includes
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

// Pico Includes
#include <pico/sem.h>
#include <pico/mutex.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>

// Library Includes
#include <psp.h>
#include <commandnet.h>
#include <sensornet.h>
#include <dma.h>
#include <spi.h>

// Device Includes
#include <w5500.h>
#include <ads13x.h>

// Function Prototypes
// void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName );
void sampleSharedResouceFunction(char* out);
void vTaskSMP_print_core();

#define task_size 128

// Buffer Initializations
StackType_t buffer_a[128];
StackType_t buffer_b[128];
StackType_t buffer_c[128];

StaticTask_t task_buffer1;
StaticTask_t task_buffer2;
StaticTask_t task_buffer3;

// Print Mutex Initialization
StaticSemaphore_t print_mutex_buffer;
SemaphoreHandle_t print_mutex = NULL;

int main() {
    stdio_init_all();
    while (!stdio_usb_connected())
        ;

    print_mutex = xSemaphoreCreateMutexStatic(&print_mutex_buffer);

    // This will be done for each task that needs to be pinned to a certain core
    // (bit mask)
    xTaskCreateStaticAffinitySet(vTaskSMP_print_core, "A", 128, NULL, 1,
                                 buffer_a, &task_buffer1, 1 << 0);
    xTaskCreateStaticAffinitySet(vTaskSMP_print_core, "B", 128, NULL, 1,
                                 buffer_b, &task_buffer2, 1 << 1);

    // This can be done for tasks that can run on either core
    xTaskCreateStatic(vTaskSMP_print_core, "C", 128, NULL, 1, buffer_c,
                      &task_buffer3);

    // Don't touch anything below here
    vTaskStartScheduler();

    while (true)
        ;

    // This point should not be reached unless vTaskEndScheduler() is called

    return EXIT_FAILURE;
}

// Semaphore has to be used anytime a shared resource is used, i.e. memory

// Safe print
void sprint(char* out) {
    xSemaphoreTake(print_mutex, portMAX_DELAY);

    puts(out);

    xSemaphoreGive(print_mutex);
}

void vTaskSMP_print_core() {
    TaskHandle_t handle = xTaskGetCurrentTaskHandle();

    UBaseType_t mask = vTaskCoreAffinityGet(handle);

    char* name = pcTaskGetName(handle);

    char out[32];

    for (;;) {
        sprintf(out, "%s  %d  %ld\n ", name, get_core_num(),
                xTaskGetTickCount());

        sprint(out);

        vTaskDelay(100);
    }
}
