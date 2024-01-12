#pragma once

#include <stdbool.h>
#include <pico/stdlib.h>
#include <w5500.h>
#include <cmdnet.h>
#include <sensornet.h>
#include <state-machine.h>
#include <ntp.h>

#include "config.h"

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

// SPI Mode (Clock Polarity and Phase setting)
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
static const uint ADC0_BAUD   = MHz(10) + KHz(200);
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

extern SemaphoreHandle_t global_mutex;

static inline void global_lock() {
    xSemaphoreTake(global_mutex, portMAX_DELAY);
}

static inline void global_unlock() {
    xSemaphoreGive(global_mutex);
}

extern sm_t state_machine;

typedef enum { BB_ISOLATE = 0, BB_OPEN, BB_REGULATE } bb_state_t;
extern bb_state_t fuel_state;
extern bb_state_t ox_state;

extern int32_t fuel_pressure;
extern int32_t ox_pressure;

extern int32_t fuel_upper_setpoint;
extern int32_t fuel_lower_setpoint;

extern int32_t ox_upper_setpoint;
extern int32_t ox_lower_setpoint;

//------------Debug Things------------

void safeprintf(const char* format, ...);
