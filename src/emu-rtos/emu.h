#pragma once

#define __STDC_FORMAT_MACROS

#include <stdbool.h>
#include <inttypes.h>
#include <pico/stdlib.h>
#include <hardware/flash.h>
#include <w5500.h>
#include <cmdnet.h>
#include <sensornet.h>
#include <state-machine.h>
#include <ntp.h>

#include "config.h"

static const uint PYRO_OFF = 0;
static const uint PYRO_ON  = 1;

static const uint PYRO_CONT_MUX_ENABLE = 11;

static const uint PYRO_0 = 17;
static const uint PYRO_1 = 16;
static const uint PYRO_2 = 15;

static const uint PYRO_CONT_MUX_A = 12;
static const uint PYRO_CONT_MUX_B = 13;
static const uint PYRO_CONT_MUX_C = 14;

static inline void pyro_cont_mux_put(uint8_t value) {
    gpio_put(PYRO_CONT_MUX_A, (value >> 0) & 1);
    gpio_put(PYRO_CONT_MUX_B, (value >> 1) & 1);
    gpio_put(PYRO_CONT_MUX_C, (value >> 2) & 1);

    // 139 ns max transition time for the mux
    // one clock is 8 ns
    // round trip signal delay < 2 ns
    // so let's wait 150 ns to be safe
    // 150 / 8 = 18.75 round up to 20

    for (int i = 0; i < 20; i++) {
        asm volatile("nop");
    }
}

static const uint8_t map_pyro_to_mux[PYRO_CONTS] = {6, 4, 1, 2, 3, 0};

// Power System Sense Lines
static const uint PS_SENSE_MUX_ENABLE = 8;

static const uint PYRO_POWER_MUX_AI = 29;

extern StackType_t task_stacks[TASK_STACK_SIZE * NUM_TASKS];
extern StaticTask_t task_buffers[NUM_TASKS];

//------------Time------------
extern int64_t offset;

static inline uint64_t unix_time_us() {
    return time_us_64() + offset;
}

//------------Devices------------

extern myspi_device_t eth0;    // W5500
extern myspi_device_t flash0;  // W25N01
extern myspi_device_t adc0;    // ADS131M06
extern myspi_device_t tc0;     // MAX31856
extern myspi_device_t tc1;     // MAX31856

// Chip select pin numbers
static const uint ETH0_CS   = 25;
static const uint FLASH0_CS = 20;
static const uint ADC0_CS   = 6;
static const uint TC0_CS    = 1;
static const uint TC1_CS    = 0;

static const uint ADC0_RESET = 7;
static const uint ADC0_DRDY  = 5;

// SPI Mode (Clock Polarity and Phase setting)F
// Specific to chip model
#define W5500_MODE    SPI_CPOL_1, SPI_CPHA_1
#define W25N01_MODE   SPI_CPOL_1, SPI_CPHA_1
#define ADS13X_MODE   SPI_CPOL_0, SPI_CPHA_1
#define MAX31856_MODE SPI_CPOL_1, SPI_CPHA_1

#define MHz(x) (1000000 * (x))
#define KHz(x) (1000 * (x))

// Baud rates, specific to device because traces
// affect SPI baud rate

// these numbers are from datasheets
// actual rates will differ, but this is the max
// difference due to how RP2040 generates clock

static const uint ETH0_BAUD   = MHz(60);  // 80 MHz was too fast
static const uint FLASH0_BAUD = MHz(100);
static const uint ADC0_BAUD   = MHz(10);
static const uint TC0_BAUD    = MHz(5);
static const uint TC1_BAUD    = MHz(5);

#undef MHz
#undef KHz

//------------CommandNet------------

extern const cmdnet_endpoint_t endpoints[];
extern const size_t n_endpoints;

//------------Constants------------

// Network
static const w5500_socket_t SENSORNET_SOCKET  = W5500_S0;
static const w5500_socket_t COMMANDNET_SOCKET = W5500_S1;

static const w5500_socket_t NTP_SOCKET = W5500_S3;

// GPIO

static const bool SOLENOID_CLOSE = 0;
static const bool SOLENOID_OPEN  = 1;

static const uint FUEL_SOLENOID = 9;
static const uint OX_SOLENOID   = 10;
static const uint AUX_SOLENOID  = 15;

//------------Tasks------------
void cmdnet_task_main();
void data_writer_main();
void sm_task_main();
void bang_bang_loop_main();

void tc0_reader_main();
void tc1_reader_main();

extern TaskHandle_t adc0_reader_task;
void adc0_reader_main();

void w5500_drdy_handler();
void adc0_drdy_isr();

//------------Data Writer------------

extern QueueHandle_t data_writer_queue;

//------------Globals------------

static inline void global_lock() {
    taskENTER_CRITICAL();
}

static inline void global_unlock() {
    taskEXIT_CRITICAL();
}

typedef enum { BB_ISOLATE = 0, BB_OPEN, BB_REGULATE } bb_state_t;

typedef union {
    struct {
        uint32_t magic;

        sm_t state_machine;

        bb_state_t fuel_state;
        bb_state_t ox_state;
        bb_state_t aux_state;

        int32_t fuel_upper_setpoint;
        int32_t fuel_lower_setpoint;

        int32_t ox_upper_setpoint;
        int32_t ox_lower_setpoint;

        int32_t aux_upper_setpoint;
        int32_t aux_lower_setpoint;
    };

    uint8_t raw[FLASH_PAGE_SIZE];
} presistent_globals_t;

extern presistent_globals_t persistent_globals;

static sm_t* const state_machine = &persistent_globals.state_machine;

extern int32_t fuel_pressure;
extern int32_t ox_pressure;
extern int32_t aux_pressure;

// Saving and Loading Flash Values

#define FLASH_MAGIC 0xcafebabe

#define FLASH_OFFSET (256 * 1024)  // 256 KiB, size of the program
static const uint8_t* stored_presistent_contents =
    (uint8_t*)(XIP_BASE + FLASH_OFFSET);

void save_presistent_globals();
void load_persistent_globals();

//------------Debug Things------------

void safeprintf(const char* format, ...);
