#include <stdbool.h>
#include <stdint.h>

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#define NUM_STATES 4

typedef uint8_t state_t;

// Setup a bit field of states, each state takes one bit and the entire length of the struct == sizeof(state_t) bytes
struct StateBitfield {
    state_t generic_state0 : 1;
    state_t generic_state1 : 1;
    state_t generic_state2 : 1;
    state_t generic_state3 : 1;
    state_t : 4;
} init_state = {true, false, true, false};

typedef struct StateBitfield State;

// Update the state of the vehicle
void update_state(State*);

// Useful print function
void print_state(State);

#endif