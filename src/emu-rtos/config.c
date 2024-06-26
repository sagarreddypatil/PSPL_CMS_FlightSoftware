#include "emu.h"
#include "config.h"

//------------CommandNet------------

#define PARAM mpack_reader_t* reader
#define CHECK_ERRORS                              \
    if (mpack_reader_error(reader) != mpack_ok) { \
        return false;                             \
    }

bool sanity_cmd_handler(PARAM) {
    static int ncheck = 0;
    safeprintf("cmdnet sanity check %d\n", ncheck++);

    return true;
}

bool sm_hold_handler(PARAM) {
    global_lock();
    sm_hold(state_machine, unix_time_us());
    global_unlock();

    return true;
}

bool sm_continue_new_t0_handler(PARAM) {
    const uint64_t new_t0 = mpack_expect_u64(reader);
    CHECK_ERRORS;

    global_lock();
    sm_continue_new_t0(state_machine, new_t0);
    global_unlock();

    return true;
}

bool sm_continue_handler(PARAM) {
    global_lock();
    sm_continue(state_machine, unix_time_us());
    global_unlock();

    return true;
}

bool sm_continue_old_t0_handler(PARAM) {
    global_lock();
    sm_continue_old_t0(state_machine);
    global_unlock();

    return true;
}

static inline bool _sm_poll_answer(sm_poll_status_t status) {
    global_lock();
    sm_poll_answer(state_machine, status, unix_time_us());
    global_unlock();

    return true;
}

bool sm_poll_answer_go_handler(PARAM) {
    return _sm_poll_answer(SM_POLL_GO);
}

bool sm_poll_answer_nogo_handler(PARAM) {
    return _sm_poll_answer(SM_POLL_NOGO);
}

typedef enum {
    BB_SYSTEM_FUEL,
    BB_SYSTEM_OX,
    BB_SYSTEM_AUX,
} bb_system_t;

bool fluid_system_set_state(bb_system_t system, bb_state_t state) {
    switch (system) {
        case BB_SYSTEM_FUEL:
            global_lock();
            persistent_globals.fuel_state = state;
            global_unlock();
            return true;
        case BB_SYSTEM_OX:
            global_lock();
            persistent_globals.ox_state = state;
            global_unlock();
            return true;
        case BB_SYSTEM_AUX:
            global_lock();
            persistent_globals.aux_state = state;
            global_unlock();
            return true;
        default:
            return false;
    }
}

bool fluid_system_set_upper_setpoint(bb_system_t system, PARAM) {
    int32_t setpoint = mpack_expect_i32(reader);
    CHECK_ERRORS;

    switch (system) {
        case BB_SYSTEM_FUEL:
            global_lock();
            persistent_globals.fuel_upper_setpoint = setpoint;
            global_unlock();
            return true;
        case BB_SYSTEM_OX:
            global_lock();
            persistent_globals.ox_upper_setpoint = setpoint;
            global_unlock();
            return true;
        case BB_SYSTEM_AUX:
            global_lock();
            persistent_globals.aux_upper_setpoint = setpoint;
            global_unlock();
            return true;
        default:
            return false;
    }
}

bool fluid_system_set_lower_setpoint(bb_system_t system, PARAM) {
    int32_t setpoint = mpack_expect_i32(reader);
    CHECK_ERRORS;

    switch (system) {
        case BB_SYSTEM_FUEL:
            global_lock();
            persistent_globals.fuel_lower_setpoint = setpoint;
            global_unlock();
            return true;
        case BB_SYSTEM_OX:
            global_lock();
            persistent_globals.ox_lower_setpoint = setpoint;
            global_unlock();
            return true;
        case BB_SYSTEM_AUX:
            global_lock();
            persistent_globals.aux_lower_setpoint = setpoint;
            global_unlock();
            return true;
        default:
            return false;
    }
}

bool bb_ox_isolate_handler(PARAM) {
    return fluid_system_set_state(BB_SYSTEM_OX, BB_ISOLATE);
}

bool bb_ox_open_handler(PARAM) {
    return fluid_system_set_state(BB_SYSTEM_OX, BB_OPEN);
}

bool bb_ox_regulate_handler(PARAM) {
    return fluid_system_set_state(BB_SYSTEM_OX, BB_REGULATE);
}

bool bb_fu_isolate_handler(PARAM) {
    return fluid_system_set_state(BB_SYSTEM_FUEL, BB_ISOLATE);
}

bool bb_fu_open_handler(PARAM) {
    return fluid_system_set_state(BB_SYSTEM_FUEL, BB_OPEN);
}

bool bb_fu_regulate_handler(PARAM) {
    return fluid_system_set_state(BB_SYSTEM_FUEL, BB_REGULATE);
}

bool bb_ox_set_upper_setpoint_handler(PARAM) {
    return fluid_system_set_upper_setpoint(BB_SYSTEM_OX, reader);
}

bool bb_ox_set_lower_setpoint_handler(PARAM) {
    return fluid_system_set_lower_setpoint(BB_SYSTEM_OX, reader);
}

bool bb_fu_set_upper_setpoint_handler(PARAM) {
    return fluid_system_set_upper_setpoint(BB_SYSTEM_FUEL, reader);
}

bool bb_fu_set_lower_setpoint_handler(PARAM) {
    return fluid_system_set_lower_setpoint(BB_SYSTEM_FUEL, reader);
}

bool bb_aux_isolate_handler(PARAM) {
    return fluid_system_set_state(BB_SYSTEM_AUX, BB_ISOLATE);
}

bool bb_aux_open_handler(PARAM) {
    return fluid_system_set_state(BB_SYSTEM_AUX, BB_OPEN);
}

bool bb_aux_regulate_handler(PARAM) {
    return fluid_system_set_state(BB_SYSTEM_AUX, BB_REGULATE);
}

bool bb_aux_set_upper_setpoint_handler(PARAM) {
    return fluid_system_set_upper_setpoint(BB_SYSTEM_AUX, reader);
}

bool bb_aux_set_lower_setpoint_handler(PARAM) {
    return fluid_system_set_lower_setpoint(BB_SYSTEM_AUX, reader);
}

bool pyro_set_state(uint pyro, uint state) {
    uint output = state ? PYRO_ON : PYRO_OFF;

    uint pin;
    switch (pyro) {
        case 0:
            pin = PYRO_0;
            break;
        case 1:
            pin = PYRO_1;
            break;
        case 2:
            pin = PYRO_2;
            break;
        default:
            return false;
    }

    gpio_put(pin, output); // this is atomic
    return true;
}

bool pyro_set_state_handler(PARAM) {
    uint pyro = mpack_expect_u8(reader);
    CHECK_ERRORS;

    uint state = mpack_expect_u8(reader);
    CHECK_ERRORS;

    return pyro_set_state(pyro, state);
}

#undef PARAM

#define CMDNET_HANDLER(endpoint_name) \
    { .name = #endpoint_name, .handler = endpoint_name##_handler }

const cmdnet_endpoint_t endpoints[] = {
    CMDNET_HANDLER(sanity_cmd),

    CMDNET_HANDLER(sm_hold),
    CMDNET_HANDLER(sm_continue_new_t0),
    CMDNET_HANDLER(sm_continue),
    CMDNET_HANDLER(sm_continue_old_t0),

    CMDNET_HANDLER(sm_poll_answer_go),
    CMDNET_HANDLER(sm_poll_answer_nogo),

    // Oxygen
    CMDNET_HANDLER(bb_ox_isolate),
    CMDNET_HANDLER(bb_ox_open),
    CMDNET_HANDLER(bb_ox_regulate),

    // Fuel
    CMDNET_HANDLER(bb_fu_isolate),
    CMDNET_HANDLER(bb_fu_open),
    CMDNET_HANDLER(bb_fu_regulate),

    // Auxiliary
    CMDNET_HANDLER(bb_aux_isolate),
    CMDNET_HANDLER(bb_aux_open),
    CMDNET_HANDLER(bb_aux_regulate),

    CMDNET_HANDLER(bb_ox_set_upper_setpoint),
    CMDNET_HANDLER(bb_ox_set_lower_setpoint),

    CMDNET_HANDLER(bb_fu_set_upper_setpoint),
    CMDNET_HANDLER(bb_fu_set_lower_setpoint),

    CMDNET_HANDLER(bb_aux_set_upper_setpoint),
    CMDNET_HANDLER(bb_aux_set_lower_setpoint),

    CMDNET_HANDLER(pyro_set_state),
};

const size_t n_endpoints = sizeof(endpoints) / sizeof(cmdnet_endpoint_t);

//------------State Machine------------

// Event Handlers

void test_event() {
    safeprintf("Test Event Reached!\n");
}