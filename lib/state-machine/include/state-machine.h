#pragma once

#include <stdint.h>
#include <pico/time.h>

typedef void (*sm_event_fn_t)(void);

typedef struct {
    uint64_t time;
    sm_event_fn_t fn;
} sm_event_t;

typedef enum {
    SM_POLL_NOGO = -1,
    SM_POLL_INIT = 0,
    SM_POLL_GO   = 1
} sm_poll_status_t;

typedef struct {
    uint64_t start_time;
    uint64_t timeout_duration;
    const char *message;  // string literal
} sm_poll_t;

typedef enum {
    SM_STATE_HOLD,
    SM_STATE_COUNTDOWN,
    SM_STATE_POLLING,
    SM_STATE_FLIGHT,
    SM_STATE_COAST
} sm_state_t;

typedef struct {
    sm_state_t state;
    uint64_t t0;  // T-0 time of the countdown, all times relative to this

    uint64_t hold_time;     // time at which the countdown was held
    sm_state_t hold_state;  // state before holding

    const sm_event_t *events;
    uint32_t num_events;

    int upcoming_event;

    const sm_poll_t *polls;
    uint32_t num_polls;

    int current_poll;
    sm_poll_status_t current_poll_status;
} sm_t;

static inline uint64_t sm_absolute_time() {
    return time_us_64();  // TODO: change to NTP time
}

void sm_init(sm_t *sm, const sm_event_t *events, uint32_t num_events,
             const sm_poll_t *polls, uint32_t num_polls);

/*
 * Time in microseconds relative to T-0
 * For example, T-1s will return -1000000
 */
int64_t sm_relative_time(sm_t *sm);

void sm_hold(sm_t *sm);

/*
 * Continue the countdown with a new T-0 time.
 * The T-0 clock will appear to jump forward/backward.
 */
void sm_continue_new_t0(sm_t *sm, uint64_t t0);

/*
 * Continue the countdown with the increased T-0 time.
 * Because T-0 gets pushed back during a hold
 */
void sm_continue(sm_t *sm);

/*
 * Continue the task with the old T-0 time
 * The T-0 clock will appear to jump forward
 */
void sm_continue_old_t0(sm_t *sm);

void sm_poll_answer(sm_t *sm, sm_poll_status_t status);

/*
 * Checks if any polls, events need to be run
 * Basically run it everywhere you change the state machine
 * Also run it in a 1ms tick
 */
void sm_run_polls_events(sm_t *sm);