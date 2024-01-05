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

myspi_device_t eth0;    // W5500
myspi_device_t flash0;  // W25N01
myspi_device_t adc0;    // ADS131M06
myspi_device_t tc0;     // MAX31856
myspi_device_t tc1;     // MAX31856

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

void setup_hardware() {
    stdio_usb_init();
    while (!stdio_usb_connected()) tight_loop_contents();
    for (int i = 0; i < 10; i++) printf("\n");

    // --- SPI --- //
    myspi_bus_init(myspi0, 2, 3, 4);
    myspi_bus_init(myspi1, 27, 26, 28);

    myspi_device_init(&eth0, myspi0, ETH0_CS, W5500_MODE, ETH0_BAUD);
    myspi_device_init(&flash0, myspi1, FLASH0_CS, W25N01_MODE, FLASH0_BAUD);
    myspi_device_init(&adc0, myspi1, ADC0_CS, ADS13X_MODE, ADC0_BAUD);
    myspi_device_init(&tc0, myspi1, TC0_CS, MAX31856_MODE, TC0_BAUD);
    myspi_device_init(&tc1, myspi1, TC1_CS, MAX31856_MODE, TC1_BAUD);
}

void init_task() {
    // --- Ethernet --- //
    myspi_device_init(&eth0, myspi1, 25U, 1, 1, 30000000);

    pico_unique_board_id_t board_id;
    pico_get_unique_board_id(&board_id);

    mac_t src_mac = {0x02,           board_id.id[3], board_id.id[4],
                     board_id.id[5], board_id.id[6], board_id.id[7]};

    printf(psp_logo);
    printf("Program: %s\n", PICO_PROGRAM_NAME);
    printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
}