/* Kernel includes. */
#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */
#include "semphr.h"   /* Semaphore related API prototypes. */

#include <stdio.h>
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

#define CreateTaskCore0(index, entrypoint, name, priority)               \
    xTaskCreateStatic(entrypoint, name, TASK_STACK_SIZE, NULL, priority, \
                      &task_stacks[(index) * TASK_STACK_SIZE],           \
                      &task_buffers[index])

void setup_hardware();
void init_task();

StaticQueue_t static_queue;
sm_t state_machine;

int main() {
    setup_hardware();

    CreateTaskCore0(0, init_task, "Initializer", 100);

    vTaskStartScheduler();

    // should not reach here
    while (true)
        ;

    return EXIT_FAILURE;
}

void init_task() {
    // --- Ethernet --- //
    myspi_device_t w5500;
    myspi_device_init(&w5500, myspi1, 25U, 27U, 26U, 28U, 1, 1, 30000000);

    pico_unique_board_id_t board_id;
    pico_get_unique_board_id(&board_id);

    mac_t src_mac = {0x02,           board_id.id[3], board_id.id[4],
                     board_id.id[5], board_id.id[6], board_id.id[7]};

    // --- ADC --- //
    myspi_device_t ads13x;
    // myspi_device_init(&ads13x, );

    //------------Poll Initialization Complete------------
    while (!w5500_ready(&w5500)) tight_loop_contents();
    while (!ads13x_ready(&ads13x)) tight_loop_contents();

    printf(psp_logo);
    printf("Program: %s\n", PICO_PROGRAM_NAME);
    printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
}

void setup_hardware() {
    stdio_usb_init();
    while (!stdio_usb_connected()) tight_loop_contents();
    for (int i = 0; i < 10; i++) printf("\n");

    /*
     * below rates are default, fast slew not yet tested, need to scope
     * if signal ripples ripples, lower the rate
     * if doesn't rise/fall to correct voltages, raise the rate
     *
     * See:
     * Falstad Circuit Simulator
     * https://tinyurl.com/2m9dxu53
     */

    // --- SPI --- //
    myspi_bus_init(myspi0);
    myspi_bus_init(myspi1);
}