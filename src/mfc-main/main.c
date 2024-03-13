/* Kernel includes. */

#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */
#include "semphr.h"   /* Semaphore related API prototypes. */

#include <stdio.h>
#include <stdarg.h>
#include <pico/stdlib.h>

#include <myspi.h>
#include <mydma.h>
#include <psp.h>

#include "mfc.h"

#define EXIT_FAILURE 1

StackType_t task_stacks[TASK_STACK_SIZE * NUM_TASKS];
StaticTask_t task_buffers[NUM_TASKS];

void task_wrapper(void* _task_entrypoint) {
    void (*task_entrypoint)(void) = _task_entrypoint;
    task_entrypoint();

    TaskHandle_t my_handle = xTaskGetCurrentTaskHandle();

    safeprintf("\nTask \"%s\" exited\n\n", pcTaskGetName(my_handle));
    vTaskDelete(my_handle);

    while (1)
        ;
}

#define CreateTaskCore0(index, entrypoint, name, priority)                    \
    xTaskCreateStatic(task_wrapper, name, TASK_STACK_SIZE, (void*)entrypoint, \
                      priority, &task_stacks[(index) * TASK_STACK_SIZE],      \
                      &task_buffers[index])

void setup_hardware();
void init_task();

TaskHandle_t adc0_reader_task;

#ifndef NDEBUG
StaticSemaphore_t print_mutex_buf;
SemaphoreHandle_t print_mutex;
#endif

myspi_device_t eth0;
myspi_device_t flash0;
myspi_device_t adc0;

uint8_t data_writer_queue_storage_buf[sizeof(sensornet_packet_t) *
                                      DATA_WRITER_QUEUE_SIZE];
StaticQueue_t data_writer_queue_buf;
QueueHandle_t data_writer_queue;

int main() {
#ifndef NDEBUG
    print_mutex = xSemaphoreCreateMutexStatic(&print_mutex_buf);
#endif

    setup_hardware();

    //------------Data Writer Queue------------
    data_writer_queue = xQueueCreateStatic(
        DATA_WRITER_QUEUE_SIZE, sizeof(sensornet_packet_t),
        data_writer_queue_storage_buf, &data_writer_queue_buf);

    //------------Initialize Globals------------

    CreateTaskCore0(0, init_task, "Initializer", 30);
    vTaskStartScheduler();

    // should not reach here
    while (true)
        ;

    return EXIT_FAILURE;
}

void setup_hardware() {

    // Initialize RP2040 GPIO pins
    gpio_init(ADC0_RESET);
    gpio_set_dir(ADC0_RESET, GPIO_OUT);
    gpio_put(ADC0_RESET, 1);

    gpio_init(ADC0_DRDY);
    gpio_set_dir(ADC0_DRDY, GPIO_IN);
    gpio_set_input_enabled(ADC0_DRDY, true);

    gpio_init(19);
    gpio_set_dir(19, GPIO_OUT);
    gpio_put(19, true);

    // Enable serial output in debug mode
#ifndef NDEBUG
    stdio_usb_init();
    while (!stdio_usb_connected()) tight_loop_contents();
    stdio_flush();
    for (int i = 0; i < 10; i++) printf("\n");
#endif

    // Initialize SPI busses 
    myspi_bus_init(myspi0, 3, 4, 2);        // TODO: add function to init pins regardless of bus
    myspi_bus_init(myspi1, 27, 28, 26);

    // Initialize SPI devices
    myspi_device_init(&eth0, myspi1, ETH0_CS, W5500_MODE, ETH0_BAUD);
    myspi_device_init(&flash0, myspi0, FLASH0_CS, W25N01_MODE, FLASH0_BAUD);
    myspi_device_init(&adc0, myspi0, ADC0_CS, ADS13X_MODE, ADC0_BAUD);
}

void init_eth0();
bool ntp_sync();

void init_task() {
    safeprintf(psp_logo);
    safeprintf("Program: %s\n", PICO_PROGRAM_NAME);
    safeprintf("Version: %s\n\n", PICO_PROGRAM_VERSION_STRING);

    init_eth0();
    safeprintf("\n");
    bool success = ntp_sync();

    if (success) {
        safeprintf("Time synced, offset: %lld\n", offset);
    } else {
        safeprintf("NTP Sync Failed! Halting\n");
        while (1) tight_loop_contents();
    }

    CreateTaskCore0(1, data_writer_main, "Data Writer", 2);

    adc0_reader_task = CreateTaskCore0(2, adc0_reader_main, "ADC0 Reader", 3);
}

// --- NTP Sync --- //
int64_t offset = 0;
bool ntp_sync() {
    // TODO: make something that'll work in flight

    int64_t new_offset = get_server_time(&eth0, NTP_SERVER_IP, NTP_SOCKET);

    if (new_offset > 0) {
        offset = new_offset;
        return true;
    }
    return false;
}

// Debug mode, thread safe print function
void safeprintf(const char* format, ...) {
#ifndef NDEBUG
    xSemaphoreTake(print_mutex, portMAX_DELAY);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    xSemaphoreGive(print_mutex);
#endif
}
