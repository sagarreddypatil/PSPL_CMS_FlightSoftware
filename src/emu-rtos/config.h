#pragma once

#include <stdint.h>
#include <sensornet.h>
#include <w5500.h>
#include <state-machine.h>

//------------Misc------------
#define TASK_STACK_SIZE 1024
#define NUM_TASKS       8

#define DATA_WRITER_QUEUE_SIZE 1024

//------------SensorNet IDs------------
static const sensornet_id_t SENSOR_ID_VEHICLE_CLOCK = 1;

//------------Network------------
static const ip_t gateway     = {192, 168, 2, 1};
static const ip_t subnet_mask = {255, 255, 255, 0};
static const ip_t src_ip      = {192, 168, 2, 50};

static const ip_t SENSORNET_IP            = {192, 168, 2, 1};
static const uint16_t SENSORNET_SRC_PORT  = 5002;
static const uint16_t SENSORNET_DEST_PORT = 5001;

static const uint16_t COMMANDNET_PORT = 8080;

//------------State Machine------------
static const uint32_t STATE_MACHINE_TICK = 10;  // milliseconds

static const sm_event_t sm_events[] = {};
static const sm_poll_t sm_polls[]   = {};

static const uint sm_events_len = sizeof(sm_events) / sizeof(sm_event_t);
static const uint sm_polls_len  = sizeof(sm_polls) / sizeof(sm_poll_t);
