#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#define SAFE        0b00000001
#define SKETCHY     0b00000010
#define RECORD      0b00000100
#define FINISHED    0b00001000

// State Byte can encode up to 255 states in a single byte instead of one byte for each state (bool)
typedef char state_byte;

// Useful print function
void print_state_byte(state_byte);

#endif