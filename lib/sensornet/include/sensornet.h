#pragma once
#include <stdint.h>

typedef uint16_t sensornet_id_t;

typedef struct {
    char type[4];
    sensornet_id_t id;
    uint64_t time_us;
    uint64_t counter;
    int64_t value;
} sensornet_packet_t;
