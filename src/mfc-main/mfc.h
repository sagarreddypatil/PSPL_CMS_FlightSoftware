#pragma once

#define __STDC_FORMAT_MACROS

#include <stdbool.h>
#include <pico/stdlib.h>
#include <hardware/flash.h>
#include <hardware/adc.h>
#include <w5500.h>
#include <sensornet.h>
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

// Chip select pin numbers
static const uint ETH0_CS   = 25;
static const uint FLASH0_CS = 0;
static const uint ADC0_CS   = 6;

static const uint ADC0_RESET = 7;
static const uint ADC0_DRDY  = 5;

// SPI Mode (Clock Polarity and Phase setting)
// Specific to chip model
#define W5500_MODE    SPI_CPOL_1, SPI_CPHA_1
#define W25N01_MODE   SPI_CPOL_1, SPI_CPHA_1
#define ADS13X_MODE   SPI_CPOL_0, SPI_CPHA_1

#define MHz(x) (1000000 * (x))
#define KHz(x) (1000 * (x))

// Baud rates, specific to device because traces
// affect SPI baud rate

// these numbers are from datasheets
// actual rates will differ, but this is the max
// difference due to how RP2040 generates clock

static const uint ETH0_BAUD   = MHz(60);  // 80 MHz was too fast
static const uint FLASH0_BAUD = MHz(100);
static const uint ADC0_BAUD   = MHz(60);

#undef MHz
#undef KHz

//------------Constants------------

// Network
static const w5500_socket_t SENSORNET_SOCKET  = W5500_S0;
static const w5500_socket_t COMMANDNET_SOCKET = W5500_S1;   // not in use on MFC
static const w5500_socket_t INTERCOM_SOCKET   = W5500_S2;
static const w5500_socket_t NTP_SOCKET        = W5500_S3;

// GPIO

//------------Tasks------------
void intercom_main();

void data_writer_main();

extern TaskHandle_t adc0_reader_task;
void adc0_reader_main();
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

// Saving and Loading Flash Values

#define FLASH_MAGIC 0xcafebabe

#define FLASH_OFFSET (256 * 1024)  // 256 KiB, size of the program
static const uint8_t* stored_presistent_contents =
    (uint8_t*)(XIP_BASE + FLASH_OFFSET);

void save_presistent_globals();
void load_persistent_globals();

//------------Debug Things------------

void safeprintf(const char* format, ...);
