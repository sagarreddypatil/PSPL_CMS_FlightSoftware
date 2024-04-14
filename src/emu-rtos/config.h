#pragma once

#include <stdint.h>
#include <sensornet.h>
#include <w5500.h>
#include <ads13x.h>
#include <state-machine.h>

//------------Misc------------
#define TASK_STACK_SIZE 2048
#define NUM_TASKS       8

#define DATA_WRITER_QUEUE_SIZE 1024

//------------Thermocouples------------

#define TC_SAMPLE_PERIOD        40   // milliseconds, 25 Hz
#define TC_INIT_FAIL_RETRY_TIME 500  // ms

//------------PT ADCs------------

// See the datasheet for info https://www.ti.com/lit/ds/symlink/ads131m06.pdf

// TODO: Actually use this sample rate
#define ADC0_READY_TIMEOUT 10000  // microseconds
#define ADC0_OSR           SR_1K  // 1 ksps
#define ADC0_RATE          1000   // sps, make sure it's same as above
#define ADC0_CHANNELS      3      // ADC is 6 channel, we only use 3

#define ADC0_FUEL_CHANNEL    0
#define ADC0_OX_CHANNEL      1
#define ADC0_CHAMBER_CHANNEL 2

//------------Bang Bang------------
#define BB_LOOP_PERIOD 1  // milliseconds

#define FUEL_SOLENOID_MIN_PERIOD 50000  // microseconds
#define OX_SOLENOID_MIN_PERIOD   50000  // microseconds
#define AUX_SOLENOID_MIN_PERIOD  50000  // microseconds

//------------SensorNet IDs------------
static const sensornet_id_t SENSOR_ID_VEHICLE_CLOCK = 1;

static const sensornet_id_t SENSOR_ID_TC0_PROBE_TEMP = 2;
static const sensornet_id_t SENSOR_ID_TC0_CJ_TEMP    = 3;
static const sensornet_id_t SENSOR_ID_TC1_PROBE_TEMP = 4;
static const sensornet_id_t SENSOR_ID_TC1_CJ_TEMP    = 5;

static const sensornet_id_t SENSOR_ID_ADC0_START = 6;  // + ADC0_CHANNELS

static const sensornet_id_t SENSOR_ID_PYRO_CONT_START = 9; // + PYRO_CONTS


// Next available: 9

//------------Network------------
static const ip_t GATEWAY_IP  = {192, 168, 2, 1};
static const ip_t SUBNET_MASK = {255, 255, 255, 0};
static const ip_t SRC_IP      = {192, 168, 2, 50};

static const ip_t NTP_SERVER_IP = {192, 168, 2, 114};

static const ip_t SENSORNET_IP            = {192, 168, 2, 114};
static const uint16_t SENSORNET_SRC_PORT  = 5002;
static const uint16_t SENSORNET_DEST_PORT = 3746;

static const uint16_t COMMANDNET_PORT = 8080;

//------------Pyros------------
#define PYRO_CONTS 6
#define PYRO_CONT_PERIOD 100 // milliseconds, = 10 HZ

//------------State Machine------------

#define MICROS(x) ((x) * 1000000)

void test_event();

static const uint32_t STATE_MACHINE_TICK = 10;  // milliseconds
static const uint32_t T_MINUS_PACKET_DIV = 10;  // every 10th state machine tick

static const sm_event_t sm_events[] = {
    {
        .time = MICROS(-10),
        .fn   = test_event,
    },
};
static const sm_poll_t sm_polls[] = {
    {
        .message          = "Test Poll",
        .start_time       = MICROS(-30),
        .timeout_duration = MICROS(15),
    },
};

static const uint sm_events_len = sizeof(sm_events) / sizeof(sm_event_t);
static const uint sm_polls_len  = sizeof(sm_polls) / sizeof(sm_poll_t);
