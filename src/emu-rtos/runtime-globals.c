#include "emu.h"
#include <hardware/flash.h>

// Under Mutex, needs to be saved to flash
presistent_globals_t presistent_globals = {
    .fuel_upper_setpoint = 0,
    .fuel_lower_setpoint = 0,

    .ox_upper_setpoint = 0,
    .ox_lower_setpoint = 0,

    .fuel_state = BB_ISOLATE,
    .ox_state   = BB_ISOLATE,
};

// Under Critical Section, don't need to save
int32_t fuel_pressure = 0;
int32_t ox_pressure   = 0;

void save_presistent_globals() {
}

void load_persistent_globals() {
}