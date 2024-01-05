#pragma once

#include <pico/stdlib.h>
#include <w5500.h>
#include <ads13x.h>
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
static const uint ADC0_BAUD   = MHz(10) + KHz(2);
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
static const uint LOX_SOLENOID = 16;
static const uint ETH_SOLENOID = 17;

static const uint PYRO = 28;

// ADC Builtin

static const uint PYRO_CONT_0       = 26;
static const uint PYRO_CONT_0_INPUT = 0;
static const uint PYRO_CONT_1       = 27;
static const uint PYRO_CONT_1_INPUT = 1;

//------------Tasks------------
void cmdnet_task_main();
void data_writer_main();
void sm_task_main();

void w5500_drdy_handler();

extern TaskHandle_t w5500_drdy_task;

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

//------------Debug Things------------

void safeprintf(const char* format, ...);
