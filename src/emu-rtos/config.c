#include "emu.h"
#include "config.h"

//------------CommandNet------------

#define PARAM mpack_reader_t* reader
#define CHECK_ERRORS                              \
    if (mpack_reader_error(reader) != mpack_ok) { \
        return false;                             \
    }

bool sm_hold_handler(PARAM) {
    global_lock();
    sm_hold(&state_machine, unix_time_us());
    global_unlock();

    return true;
}

bool sm_continue_new_t0_handler(PARAM) {
    const uint64_t new_t0 = mpack_expect_u64(reader);
    CHECK_ERRORS;

    global_lock();
    sm_continue_new_t0(&state_machine, new_t0);
    global_unlock();

    safeprintf("Set new T-0 to %" PRId64 "\n", new_t0);

    return true;
}

bool sm_continue_handler(PARAM) {
    global_lock();
    sm_continue(&state_machine, unix_time_us());
    global_unlock();

    return true;
}

bool sm_continue_old_t0_handler(PARAM) {
    global_lock();
    sm_continue_old_t0(&state_machine);
    global_unlock();

    return true;
}

static inline bool _sm_poll_answer(sm_poll_status_t status) {
    global_lock();
    sm_poll_answer(&state_machine, status, unix_time_us());
    global_unlock();

    return true;
}

bool sm_poll_answer_go_handler(PARAM) {
    return _sm_poll_answer(SM_POLL_GO);
}

bool sm_poll_answer_nogo_handler(PARAM) {
    return _sm_poll_answer(SM_POLL_NOGO);
}

#undef PARAM

#define CMDNET_HANDLER(endpoint_name) \
    { .name = #endpoint_name, .handler = endpoint_name##_handler }

const cmdnet_endpoint_t endpoints[] = {
    CMDNET_HANDLER(sm_hold),           CMDNET_HANDLER(sm_continue_new_t0),
    CMDNET_HANDLER(sm_continue),       CMDNET_HANDLER(sm_continue_old_t0),

    CMDNET_HANDLER(sm_poll_answer_go), CMDNET_HANDLER(sm_poll_answer_nogo),
};

const size_t n_endpoints = sizeof(endpoints) / sizeof(cmdnet_endpoint_t);

//------------State Machine------------

// Event Handlers

void test_event() {
    safeprintf("Test Event Reached!\n");
}