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
    Demonstrating how to use the "state_byte" to convey the state of the system

    Why are we using this "state_byte"?
        The state_byte allows us to have 1 byte store up to 8 states instead of using booleans, in which case each state uses 1 byte
        Basically we are using each bit as a boolean, if we don't wanna do this we should decide soon cause the state machine is a big part lol
        The rocket can also be in more than one state at the same time and it's easy to do that with the state byte

    Right now, there are only four states to demonstrate using the state byte: safe, sketchy, record and finished

    Here's a small state map for the kids at home:

        <---> means we can go back and forth,
        ----> means we can only go one way,
        <===> means we can be in both states at the same time

        SAFE <---> SKETCHY <===> RECORD ----> FINISHED <===> SAFE

        I can make a more detailed one because a one line state map doesn't give great detail on all paths but you get the gist

    The folling are various examples of how to use and manipulate the state byte

    NOTE!!!
        This is very much thrown together and can be optimized and whatnot so if you don't like the state byte because of this implementation then think again!
    */

    // Initialize the state byte in the safe state
    state_byte state = SAFE;
    

    // This is how you evaluate the state, b
    if((state & SAFE) == SAFE && (state & FINISHED) != FINISHED) {
        print_state_byte(state);
    }

    // Put the state byte into an invalid state, we'd have logic to get to this point not just a statement like this
    state += SKETCHY;
    if((state & SAFE) == SAFE && (state & SKETCHY) == SKETCHY && (state & FINISHED) != FINISHED) {
        state = SKETCHY;
        print_state_byte(state);
    }

    // Here's a condition where we can be in more than one state, but where there are some states we don't wanna be in
    // There's a cleaner way to do this don't worry it's just 11PM and I have to wake up early for work in the morning, turn that mindset into a grindset
    state += RECORD;
    if((state & RECORD) == RECORD && (state & SAFE) != SAFE && (state & FINISHED) != FINISHED) {
        print_state_byte(state);
    }

    // This is an example of entering the finished state where we cannot go back from
    state += FINISHED;
    if((state & FINISHED) == FINISHED) {
        state = FINISHED + SAFE;
        print_state_byte(state);
    }
}

void print_state_byte(state_byte state) {
    // Yes I know this function is written poorly get over it
    int reverse = 0;

    for(int rev_idx = 0; rev_idx < sizeof(state) * 8; rev_idx++) {
        reverse <<= 1;
        if (state & 1 == 1)
            reverse ^= 1;
        state >>= 1;
    }

    printf("State Byte: ");
    for(int idx = 0; idx < sizeof(state) * 8; idx++) {
        printf("%d", reverse % 2);
        reverse /= 2;
    }
    printf("\n");
}
