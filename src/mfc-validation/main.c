#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <pico/unique_id.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <myspi.h>
#include <psp.h>

#include "mfc.h"

myspi_device_t eth0;
myspi_device_t flash0;
myspi_device_t adc0;

StackType_t task_stack[1024];
StaticTask_t task_buffer;

StaticSemaphore_t print_mutex_buf;
SemaphoreHandle_t print_mutex;

void mfc_test();

int main() {

    // Enable serial output
    stdio_usb_init();
    while (!stdio_usb_connected());
    for (size_t i = 0; i < 7; i++) printf("\n");
    //-----------------------------------------------------------------------//

    // Initialize ADC reset pin
    gpio_init(ADC0_RESET);
    gpio_set_dir(ADC0_RESET, GPIO_OUT);
    gpio_put(ADC0_RESET, 1);

    // Initialize ADC data ready pin
    gpio_init(ADC0_DRDY);
    gpio_set_dir(ADC0_DRDY, GPIO_IN);
    gpio_set_input_enabled(ADC0_DRDY, true);

    // Initialize RTD activate(?) pin
    gpio_init(19);
    gpio_set_dir(19, GPIO_OUT);
    gpio_put(19, true);

    // Initialize SPI busses
    myspi_bus_init(myspi0, 3, 4, 2);
    myspi_bus_init(myspi1, 27, 28, 26);

    // Initialize SPI devices
    myspi_device_init(&eth0, myspi1, ETH0_CS, W5500_MODE, ETH0_BAUD);
    myspi_device_init(&flash0, myspi0, FLASH0_CS, W25N01_MODE, FLASH0_BAUD);
    myspi_device_init(&adc0, myspi0, ADC0_CS, ADS13X_MODE, ADC0_BAUD);

    //-----------------------------------------------------------------------//
    // Create one and only task
    xTaskCreateStatic(mfc_test, "mfc_test", 1024, NULL, 30, task_stack, &task_buffer);

    // Start scheduler...
    vTaskStartScheduler();

    // should NEVER reach here
    while(true);

    return -1;
}

// Initializes ethernet chip
void init_eth0() {

    pico_unique_board_id_t board_id;
    pico_get_unique_board_id(&board_id);

    const mac_t src_mac = {0x02, board_id.id[3], board_id.id[4], board_id.id[5], board_id.id[6], board_id.id[7]};

    myspi_lock(&eth0);
    myspi_configure(&eth0);

    printf("Readying W5500... ");
    w5500_reset(&eth0);
    while (!w5500_ready(&eth0)) tight_loop_contents();
    printf("Done.\n");

    printf("Checking for link... ");
    while (!w5500_has_link(&eth0)) tight_loop_contents();
    printf("Done.\n");


    ip_t w5500_ip;
    w5500_config(&eth0, src_mac, SRC_IP, SUBNET_MASK, GATEWAY_IP);
    w5500_read(&eth0, W5500_COMMON, W5500_SIPR0, w5500_ip, sizeof(w5500_ip));
    printf("Connected as: %d.%d.%d.%d\n\n", w5500_ip[0], w5500_ip[1], w5500_ip[2], w5500_ip[3]);

    myspi_unlock(&eth0);

}

// Initializes the ADC
void init_adc0() {

    myspi_lock(&adc0);
    myspi_configure(&adc0);

    printf("Readying ADC... ");
    ads13x_reset(&adc0);
    while (!ads13x_ready(&adc0)) tight_loop_contents();
    printf("Done.\n");

    printf("Initializing ADC... ");
    while (!ads13x_init(&adc0)) tight_loop_contents();
    printf("Done.\n");
    
    myspi_unlock(&adc0);
    
}

void mfc_test() {
    init_eth0();
    init_adc0();

    while(true);
}