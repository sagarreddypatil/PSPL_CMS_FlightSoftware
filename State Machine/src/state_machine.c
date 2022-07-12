/*
Built by Mr. Dr. Lord Master God Emperor Supreme Chancelor Overlord etc etc etc Big Cameron himself

Here Big Cameron from the planet Earth first started coding CMS, July 10th, 2022

We came in hate for Blue Origin

Starting this project was kinda like this: https://youtu.be/vFUx_KC1bHQ?t=77
*/

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
    

    if(state.safe && !state.finished) {
        print_state(state);
    }

    state.danger = true;

    if(state.safe && state.danger) {
        state.safe = false;
        state.record = true;
        print_state(state);
    }

    state.finished = true;

    if(state.finished) {
        state.safe = true;
        state.danger = false;
        state.record = false;
        print_state(state);
    }

}

void print_state(State state) {
    printf("\nSafe: %d\n", state.safe);
    printf("Danger: %d\n", state.danger);
    printf("Recording: %d\n", state.record);
    printf("Finished: %d\n\n", state.finished);
}
