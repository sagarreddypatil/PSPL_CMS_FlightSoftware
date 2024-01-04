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
#include <mydma.h>
#include <myspi.h>

// Device Includes
#include <w5500.h>
#include <ads13x.h>

// Function Prototypes
// void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName );
void vTaskSMP_print_core();
void queueReceiveTask();
void queueSendTask();

#define task_size 1024

// Buffer Initializations
StackType_t buffer_a[task_size];
StackType_t buffer_b[task_size];
StackType_t buffer_c[task_size];
uint8_t QueueStorage[task_size * 1];

StaticTask_t task_buffer1;
StaticTask_t task_buffer2;
StaticTask_t task_buffer3;
StaticQueue_t static_queue;

// Print Mutex Initialization
StaticSemaphore_t print_mutex_buffer;
SemaphoreHandle_t print_mutex = NULL;
QueueHandle_t queue;

int main() {
    stdio_init_all();
    while (!stdio_usb_connected())
        ;

    stdio_flush();

    print_mutex = xSemaphoreCreateMutexStatic(&print_mutex_buffer);

    queue = xQueueCreateStatic(task_size, 1, QueueStorage, &static_queue);

    // This will be done for each task that needs to be pinned to a certain core
    // (bit mask)
    xTaskCreateStaticAffinitySet(queueSendTask, "A", task_size, NULL, 1,
                                 buffer_a, &task_buffer1, 1 << 0);
    xTaskCreateStaticAffinitySet(queueReceiveTask, "B", task_size, NULL, 1,
                                 buffer_b, &task_buffer2, 1 << 1);

    // This can be done for tasks that can run on either core
    xTaskCreateStatic(vTaskSMP_print_core, "C", task_size, NULL, 1, buffer_c,
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
void sprint(char *out) {
    xSemaphoreTake(print_mutex, 1000);

    puts(out);

    xSemaphoreGive(print_mutex);
}

void vTaskSMP_print_core() {
    TickType_t starting_tick = xTaskGetTickCount();
    TaskHandle_t handle      = xTaskGetCurrentTaskHandle();

    char *name = pcTaskGetName(handle);

    char out[32];

    while (true) {
        sprintf(out, "%s  %d  %ld\n ", name, get_core_num(),
                xTaskGetTickCount());

        sprint(out);

        vTaskDelayUntil(&starting_tick, 1000);
    }
}

void queueSendTask() {
    TickType_t starting_tick = xTaskGetTickCount();
    TaskHandle_t handle      = xTaskGetCurrentTaskHandle();

    char *name = pcTaskGetName(handle);

    char sender = 33;
    char out[128];
    char *strpt = &sender;

    while (true) {
        sprintf(out, "Input: %c     Core: %d    Tick: %ld ", sender,
                get_core_num(), xTaskGetTickCount());

        xQueueSend(queue, strpt, pdMS_TO_TICKS(1000));
        sender++;

        sprint(out);

        vTaskDelayUntil(&starting_tick, 1000);
    }
}
void queueReceiveTask() {
    TickType_t starting_tick = xTaskGetTickCount();
    TaskHandle_t handle      = xTaskGetCurrentTaskHandle();

    char *name = pcTaskGetName(handle);

    char out[128];
    char receiver[1];

    while (true) {
        xQueueReceive(queue, receiver, pdMS_TO_TICKS(1000));

        // sprintf(out, "%s  %d  %ld  %s\n ", name, get_core_num(),
        // xTaskGetTickCount(), receiver);
        sprintf(out, "Output: %c    Core: %d    Tick: %ld ", receiver[0],
                get_core_num(), xTaskGetTickCount());

        sprint(out);

        vTaskDelayUntil(&starting_tick, 1000);
    }
}