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
                      priority, &task_stacks[(index)*TASK_STACK_SIZE],        \
                      &task_buffers[index])

void setup_hardware();
void init_task();

TaskHandle_t adc0_reader_task;

StaticSemaphore_t print_mutex_buf;
SemaphoreHandle_t print_mutex;

myspi_device_t eth0;
myspi_device_t flash0;
myspi_device_t adc0;
myspi_device_t tc0;
myspi_device_t tc1;

uint8_t data_writer_queue_storage_buf[sizeof(sensornet_packet_t) *
                                      DATA_WRITER_QUEUE_SIZE];
StaticQueue_t data_writer_queue_buf;
QueueHandle_t data_writer_queue;

StaticSemaphore_t global_mutex_buf;
SemaphoreHandle_t global_mutex;
sm_t state_machine;

int main() {
    print_mutex = xSemaphoreCreateMutexStatic(&print_mutex_buf);

    setup_hardware();

    //------------Data Writer Queue------------
    data_writer_queue = xQueueCreateStatic(
        DATA_WRITER_QUEUE_SIZE, sizeof(sensornet_packet_t),
        data_writer_queue_storage_buf, &data_writer_queue_buf);

    //------------Initialize Globals------------
    global_mutex = xSemaphoreCreateMutexStatic(&global_mutex_buf);
    sm_init(&state_machine, sm_events, sm_events_len, sm_polls, sm_polls_len);

    CreateTaskCore0(0, init_task, "Initializer", 30);
    vTaskStartScheduler();

    // should not reach here
    while (true)
        ;

    return EXIT_FAILURE;
}

void setup_hardware() {
    gpio_init(ADC0_RESET);
    gpio_set_dir(ADC0_RESET, GPIO_OUT);
    gpio_put(ADC0_RESET, 1);

    gpio_init(ADC0_DRDY);
    gpio_set_dir(ADC0_DRDY, GPIO_IN);
    gpio_set_input_enabled(ADC0_DRDY, true);

    gpio_init(FUEL_SOLENOID);
    gpio_set_dir(FUEL_SOLENOID, GPIO_OUT);
    gpio_put(FUEL_SOLENOID, SOLENOID_CLOSE);

    gpio_init(OX_SOLENOID);
    gpio_set_dir(OX_SOLENOID, GPIO_OUT);
    gpio_put(OX_SOLENOID, SOLENOID_CLOSE);

    gpio_init(AUX_SOLENOID);
    gpio_set_dir(AUX_SOLENOID, GPIO_OUT);
    gpio_put(AUX_SOLENOID, SOLENOID_CLOSE);

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
bool ntp_sync();

void init_task() {
    safeprintf(psp_logo);
    safeprintf("Program: %s\n", PICO_PROGRAM_NAME);
    safeprintf("Version: %s\n\n", PICO_PROGRAM_VERSION_STRING);

    init_eth0();
    safeprintf("\n");
    bool success = ntp_sync();

    if (success) {
        safeprintf("Time synced, offset: %" PRId64 "\n", offset);
    } else {
        safeprintf("NTP Sync Failed! Halting\n");
        while (1) tight_loop_contents();
    }

    CreateTaskCore0(1, cmdnet_task_main, "CommandNet", 1);

    CreateTaskCore0(2, data_writer_main, "Data Writer", 2);
    CreateTaskCore0(3, sm_task_main, "State Machine", 10);  // high priority

    CreateTaskCore0(4, tc0_reader_main, "TC0 Reader", 5);
    // CreateTaskCore0(5, tc1_reader_main, "TC1 Reader", 5);

    adc0_reader_task = CreateTaskCore0(6, adc0_reader_main, "ADC0 Reader", 6);

    // CreateTaskCore0(4, ntp_test_main, "NTP Test", 1);
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

    {
        uint64_t start = time_us_64();
        uint count     = 0;
        uint delay     = 1;

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
    w5500_config(&eth0, src_mac, SRC_IP, SUBNET_MASK, GATEWAY_IP);
    w5500_read(&eth0, W5500_COMMON, W5500_SIPR0, ip, sizeof(ip));
    myspi_unlock(&eth0);

    safeprintf("Ethernet Connected, IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2],
               ip[3]);
}

// --- NTP Sync --- //
int64_t offset = 0;
bool ntp_sync() {
    // TODO: make something that'll work in flight

    myspi_lock(&eth0);
    int64_t new_offset = get_server_time(&eth0, NTP_SERVER_IP, NTP_SOCKET);
    myspi_unlock(&eth0);

    if (new_offset > 0) {
        offset = new_offset;
        return true;
    }
    return false;
}

void safeprintf(const char* format, ...) {
    xSemaphoreTake(print_mutex, portMAX_DELAY);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    xSemaphoreGive(print_mutex);
}
