/*
Built by Mr. Dr. Lord Master God Emperor Supreme Chancelor Overlord etc etc etc Big Cameron himself

Here Big Cameron from the planet Earth first started coding CMS, July 10th, 2022

We came in hate for Blue Origin

Starting this project was kinda like this: https://youtu.be/vFUx_KC1bHQ?t=77
*/

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <stdio.h>
#include "../lib/state_machine.h"

int main(int argc, char* argv[]) {
    /*
    Super basic state implementation

    It actually won't ever get much more complicated than this

    The logic behind it should always be simple, very important to be able to clearly trace your way from state to state
    */
    // Initialize the state byte in the safe state
    State state = init_state;

    while(1 == 1) {
        print_state(state);
        update_state(&state);
        Sleep(1000);
    }

    return EXIT_SUCCESS;
}

void print_state(State state) {
    char* state_names[NUM_STATES] = {"State 0: ", "State 1: ", "State 2: ", "State 3: "};
    state_t states[NUM_STATES] = {state.generic_state0, state.generic_state1, state.generic_state2, state.generic_state3};
    for(uint8_t state_idx = 0; state_idx < NUM_STATES; state_idx++) {
        printf("%s%d\n", state_names[state_idx], states[state_idx]);
    }
    printf("\n");
}

void update_state(State* state) {
    state->generic_state0 -= 1;
    state->generic_state1 += 1;
    state->generic_state2 -= 1;
    state->generic_state3 += 1;
}
