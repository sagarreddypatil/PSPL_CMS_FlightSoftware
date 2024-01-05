/* Kernel includes. */
#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */
#include "semphr.h"   /* Semaphore related API prototypes. */

#include <stdio.h>
#include <stdarg.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/unique_id.h>

#include <myspi.h>
#include <mydma.h>
#include <psp.h>

#include "emu.h"

#define TASK_STACK_SIZE 1024
#define NUM_TASKS       8

StackType_t task_stacks[TASK_STACK_SIZE * NUM_TASKS];
StaticTask_t task_buffers[NUM_TASKS];

void task_wrapper(void* _task_entrypoint) {
    void (*task_entrypoint)(void) = _task_entrypoint;
    task_entrypoint();

    TaskHandle_t my_handle = xTaskGetCurrentTaskHandle();

    safeprintf("Task \"%s\" exited\n", pcTaskGetName(my_handle));
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

myspi_device_t eth0;
myspi_device_t flash0;
myspi_device_t adc0;
myspi_device_t tc0;
myspi_device_t tc1;

StaticQueue_t static_queue;
sm_t state_machine;

StaticSemaphore_t print_mutex_buf;
SemaphoreHandle_t print_mutex;

void safeprintf(const char* format, ...) {
    xSemaphoreTake(print_mutex, portMAX_DELAY);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    xSemaphoreGive(print_mutex);
}

int main() {
    print_mutex = xSemaphoreCreateMutexStatic(&print_mutex_buf);

    setup_hardware();

    CreateTaskCore0(0, init_task, "Initializer", 30);

    vTaskStartScheduler();

    // should not reach here
    while (true)
        ;

    return EXIT_FAILURE;
}

void setup_hardware() {
    stdio_usb_init();
    while (!stdio_usb_connected()) tight_loop_contents();
    stdio_flush();
    for (int i = 0; i < 10; i++) printf("\n");

    // --- SPI --- //
    myspi_bus_init(myspi0, 2, 3, 4);
    myspi_bus_init(myspi1, 27, 26, 28);

    myspi_device_init(&eth0, myspi1, ETH0_CS, W5500_MODE, ETH0_BAUD);
    myspi_device_init(&flash0, myspi0, FLASH0_CS, W25N01_MODE, FLASH0_BAUD);
    myspi_device_init(&adc0, myspi0, ADC0_CS, ADS13X_MODE, ADC0_BAUD);
    myspi_device_init(&tc0, myspi0, TC0_CS, MAX31856_MODE, TC0_BAUD);
    myspi_device_init(&tc1, myspi0, TC1_CS, MAX31856_MODE, TC1_BAUD);
}

void init_eth0();

void init_task() {
    safeprintf(psp_logo);
    safeprintf("Program: %s\n", PICO_PROGRAM_NAME);
    safeprintf("Version: %s\n\n", PICO_PROGRAM_VERSION_STRING);

    init_eth0();

    CreateTaskCore0(1, cmdnet_task_entrypoint, "CommandNet", 1);
}

void init_eth0() {
    // TODO: make this a separate task

    // --- Ethernet --- //

    pico_unique_board_id_t board_id;
    pico_get_unique_board_id(&board_id);

    const mac_t src_mac = {0x02,           board_id.id[3], board_id.id[4],
                           board_id.id[5], board_id.id[6], board_id.id[7]};

    myspi_lock(&eth0);
    myspi_configure(&eth0);  // only need to do this once, as it's the only
                             // device on this bus

    w5500_reset(&eth0);
    while (!w5500_ready(&eth0)) tight_loop_contents();

    myspi_unlock(&eth0);

    uint64_t start = time_us_64();

    {
        uint count = 0;
        uint delay = 1;

        while (true) {
            count++;
            vTaskDelay(pdMS_TO_TICKS(delay));
            delay *= 2;

            myspi_lock(&eth0);
            if (w5500_has_link(&eth0)) {
                myspi_unlock(&eth0);
                break;
            }
            myspi_unlock(&eth0);
        }

        safeprintf("W5500 has link, took %d us after %d tries\n",
                   (int)(time_us_64() - start), count);
    }

    ip_t ip;
    myspi_lock(&eth0);
    w5500_config(&eth0, src_mac, src_ip, subnet_mask, gateway);
    w5500_read(&eth0, W5500_COMMON, W5500_SIPR0, ip, sizeof(ip));
    myspi_unlock(&eth0);

    safeprintf("Ethernet Connected, IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2],
               ip[3]);
}