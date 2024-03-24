#pragma once

#include <stdbool.h>
#include <pico/stdlib.h>
#include <hardware/flash.h>
#include <hardware/adc.h>
#include <w5500.h>
#include <ads13x.h>
#include <sensornet.h>
#include <ntp.h>

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
//------------PT ADCs------------

// See the datasheet for info https://www.ti.com/lit/ds/symlink/ads131m06.pdf

// TODO: Actually use this sample rate
#define ADC0_READY_TIMEOUT 100000  // microseconds
#define ADC0_OSR           SR_1K  // 1 ksps
#define ADC0_RATE          1000   // samples per second, make sure it's same as above
#define ADC0_CHANNELS      4      // ADC is 6 channel, we only have 5 wired

//------------SensorNet IDs------------
static const sensornet_id_t SENSOR_ID_ADC0_START = 13;  // + ADC0_CHANNELS


//------------Network------------
static const ip_t GATEWAY_IP  = {192, 168, 2, 1};
static const ip_t SUBNET_MASK = {255, 255, 255, 0};
static const ip_t SRC_IP      = {192, 168, 2, 50}; // TODO: need to be able to change this but ntp gets sad when you do

static const ip_t NTP_SERVER_IP = {192, 168, 2, 114};
static const ip_t SENSORNET_IP  = {192, 168, 2, 114};
static const ip_t INTERCOM_IP   = {192, 168, 2, 114};

static const uint16_t SENSORNET_SRC_PORT  = 5002; 
static const uint16_t SENSORNET_DEST_PORT = 3746;

static const uint16_t INTERCOM_SRC_PORT  = 2003; 
static const uint16_t INTERCOM_DEST_PORT = 1919;