#pragma once

#include <stdint.h>
#include <sensornet.h>
#include <w5500.h>
#include <ads13x.h>

//------------Misc------------
#define TASK_STACK_SIZE 2048
#define NUM_TASKS       2

#define DATA_WRITER_QUEUE_SIZE 1024

//------------PT ADCs------------

// See the datasheet for info https://www.ti.com/lit/ds/symlink/ads131m06.pdf

// TODO: Actually use this sample rate
#define ADC0_READY_TIMEOUT 10000  // microseconds
#define ADC0_OSR           SR_1K  // 1 ksps
#define ADC0_RATE          1000   // sps, make sure it's same as above
#define ADC0_CHANNELS      5      // ADC is 6 channel, we only use 3

#define ADC0_CHAMBER_CHANNEL 2

//------------SensorNet IDs------------
static const sensornet_id_t SENSOR_ID_ADC0_START = 6;  // + ADC0_CHANNELS

// Next available: 9

//------------Network------------
static const ip_t GATEWAY_IP  = {192, 168, 2, 1};
static const ip_t SUBNET_MASK = {255, 255, 255, 0};
static const ip_t SRC_IP      = {192, 168, 2, 50};

static const ip_t NTP_SERVER_IP = {192, 168, 2, 115};

static const ip_t SENSORNET_IP            = {192, 168, 2, 115};
static const uint16_t SENSORNET_SRC_PORT = 5002; 
static const uint16_t SENSORNET_DEST_PORT = 3746;
