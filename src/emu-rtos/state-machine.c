#include "emu.h"
#include "state_machine.h"

#include <assert.h>

void sm_init(sm_t *sm, const sm_event_t *events, uint32_t num_events,
             const sm_poll_t *polls, uint32_t num_polls) {
    // this function will only be called once, so no need to lock for it
    sm->mutex = xSemaphoreCreateMutexStatic(&sm->mutex_buf);

    sm->state = SM_STATE_HOLD;
    sm->t0    = 0;

    sm->hold_time  = 0;
    sm->hold_state = SM_STATE_COUNTDOWN;

    sm->events     = events;
    sm->num_events = num_events;

    sm->upcoming_event = 0;  // the first upcoming event is the first one

    sm->polls     = polls;
    sm->num_polls = num_polls;

    sm->current_poll        = 0;  // start on the first poll
    sm->current_poll_status = SM_POLL_INIT;
}

void sm_hold(sm_t *sm) {
    sm->state = SM_STATE_HOLD;

    sm->hold_state = sm->state;
    sm->hold_time  = sm_absolute_time();
}

void sm_continue_new_t0(sm_t *sm, uint64_t t0) {
    if (sm->state == SM_STATE_HOLD) return;

    sm_state_t continue_state = sm->hold_state;
    if (continue_state == SM_STATE_HOLD) {
        // this is very bad and should never happen
        continue_state = SM_STATE_COUNTDOWN;
    }

    sm->state = sm->hold_state;
    sm->t0    = t0;

    sm_run_polls_events(sm);
}

void sm_continue(sm_t *sm) {
    uint64_t time_at_continue = sm_absolute_time();
    uint64_t time_held        = time_at_continue - sm->hold_time;

    uint64_t new_t0 = sm->t0 + time_held;

    sm_continue_new_t0(sm, new_t0);
}

void sm_continue_old_t0(sm_t *sm) {
    sm_continue_new_t0(sm, sm->t0);
}

int64_t sm_relative_time(sm_t *sm) {
    if (sm->state == SM_STATE_HOLD) {
        // countdown stays static during hold
        // it's held at the time at which it was held

        return sm->t0 - sm->hold_time;
    }

    return (int64_t)sm->t0 - sm_absolute_time();
}

void sm_poll_answer(sm_t *sm, sm_poll_status_t status) {
    if (sm->state != SM_STATE_POLLING) return;
    sm->current_poll_status = status;

    sm_run_polls_events(sm);
}

void sm_run_polls_events(sm_t *sm) {
    // ALWAYS CHECK FOR POLLS BEFORE EVENTS

    if (sm->state == SM_STATE_HOLD) {
        return;
    }

    if (sm->state == SM_STATE_POLLING) {
        assert(sm->current_poll >= 0);  // if polling, we should have a poll
        assert(sm->current_poll < sm->num_polls);  // poll should be in range

        const sm_poll_t *poll = &sm->polls[sm->current_poll];

        if (sm->current_poll_status == SM_POLL_GO) {
            /*
             * The only way we move on to the next poll is answering Go
             * to the current poll
             */

            // poll is Go, move on to next poll and go back to countdown
            sm->current_poll++;
            sm->current_poll_status = SM_POLL_INIT;

            sm->state = SM_STATE_COUNTDOWN;
            return;
        }

        if (sm->current_poll_status == SM_POLL_NOGO) {
            // poll is No Go, hold countdown

            sm_hold(sm);
            return;
        }

        if (sm_relative_time(sm) > poll->start_time + poll->timeout_duration) {
            // poll is INIT + timed out, hold countdown

            sm_hold(sm);
            return;
        }
    }

    if (sm->state == SM_STATE_COUNTDOWN) {
        // check if poll
        if (sm->current_poll < sm->num_polls) {
            /*
             * Note, if the countdown is continued without answering the poll
             * then we get here without the poll index incrementing.
             *
             * So, once tick runs again, we'll notice that the poll is timed
             * out and go into a hold again
             *
             * This is the desired behavior
             */

            const sm_poll_t *poll = &sm->polls[sm->current_poll];

            if (sm_relative_time(sm) >= poll->start_time) {
                // we should be in a poll, switch state
                sm->state = SM_STATE_POLLING;
                return;
            }
        }

        // check if event
        if (sm->upcoming_event < sm->num_events) {
            const sm_event_t *event = &sm->events[sm->upcoming_event];

            if (sm_relative_time(sm) >= event->time) {
                // run the event
                event->fn();

                // move on to next event
                sm->upcoming_event++;
                return;
            }
        }

        if (sm_relative_time(sm) >= 0) {
            // countdown is over, go to flight

            sm->state = SM_STATE_FLIGHT;
            return;
        }
    }
}

void sm_task_fn() {
    DEBUGPRINT;

    uint64_t time_packet_counter = 0;
    TickType_t prev_wake         = xTaskGetTickCount();

    DEBUGPRINT;

    while (1) {
        DEBUGPRINT;
        sm_run_polls_events(&state_machine);

        // send a packet to w5500, unbuffered because this should be fast
        int64_t relative_time     = sm_relative_time(&state_machine);
        sensornet_packet_t packet = {
            .id      = SENSOR_ID_VEHICLE_CLOCK,
            .time_us = sm_absolute_time(),
            .counter = time_packet_counter++,
            .value   = relative_time,
        };
        DEBUGPRINT;

        // myspi_lock(&w5500);
        // myspi_configure(&w5500);
        // w5500_write_data(&w5500, SENSORNET_SOCKET, &packet,
        //                  sizeof(sensornet_packet_t));  // dk if it made it
        // myspi_unlock(&w5500);

        safeprintf("T-minus Clock: %" PRId64 "\n", relative_time);

        xTaskDelayUntil(&prev_wake, pdMS_TO_TICKS(1));
        DEBUGPRINT;
    }
}