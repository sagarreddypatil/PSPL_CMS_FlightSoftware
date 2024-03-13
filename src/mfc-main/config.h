#pragma once

#include <stdint.h>
#include <sensornet.h>
#include <w5500.h>
#include <ads13x.h>

//------------Misc------------
#define TASK_STACK_SIZE 2048
#define NUM_TASKS       3      // Initializing task is included

#define DATA_WRITER_QUEUE_SIZE 1024

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

static const ip_t SENSORNET_IP            = {192, 168, 2, 114};

static const uint16_t SENSORNET_SRC_PORT  = 5002; 
static const uint16_t SENSORNET_DEST_PORT = 3746;