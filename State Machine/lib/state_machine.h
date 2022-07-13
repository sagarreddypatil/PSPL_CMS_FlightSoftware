#include <stdbool.h>

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

struct StateStruct {
    bool safe;
    bool danger;
    bool record;
    bool finished;
} init_state = {true, false, false, false};

// State Byte can encode up to 255 states in a single byte instead of one byte for each state (bool)
typedef struct StateStruct State;

// Useful print function
void print_state(State);

#endif