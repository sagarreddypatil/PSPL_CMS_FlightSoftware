#pragma once

#include <stdint.h>
#include <sensornet.h>
#include <w5500.h>
#include <state-machine.h>

//------------Misc------------
#define TASK_STACK_SIZE 1024
#define NUM_TASKS       8

#define DATA_WRITER_QUEUE_SIZE 1024

//------------Timing------------

#define TC_SAMPLE_PERIOD 40  // milliseconds, 25 Hz

#define TC_INIT_FAIL_RETRY_TIME 500  // ms

//------------SensorNet IDs------------
static const sensornet_id_t SENSOR_ID_VEHICLE_CLOCK = 1;

static const sensornet_id_t SENSOR_ID_TC0_PROBE_TEMP = 2;
static const sensornet_id_t SENSOR_ID_TC0_CJ_TEMP    = 3;
static const sensornet_id_t SENSOR_ID_TC1_PROBE_TEMP = 4;
static const sensornet_id_t SENSOR_ID_TC1_CJ_TEMP    = 5;

//------------Network------------
static const ip_t GATEWAY_IP  = {192, 168, 2, 1};
static const ip_t SUBNET_MASK = {255, 255, 255, 0};
static const ip_t SRC_IP      = {192, 168, 2, 50};

static const ip_t NTP_SERVER_IP = {192, 168, 2, 1};

static const ip_t SENSORNET_IP            = {192, 168, 2, 1};
static const uint16_t SENSORNET_SRC_PORT  = 5002;
static const uint16_t SENSORNET_DEST_PORT = 5001;

static const uint16_t COMMANDNET_PORT = 8080;

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
