#include <stdint.h>

typedef struct {
    char type[4];
    uint16_t id;
    uint64_t time_us;
    uint64_t counter;
    int64_t value;
} sensornet_packet_t;