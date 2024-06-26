#include "state-machine.h"
#include "sensornet.h"
#include <assert.h>

void sm_init(sm_t *sm, const sm_event_t *events, uint32_t num_events,
             const sm_poll_t *polls, uint32_t num_polls) {
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

void safeprintf(const char *, ...);

void sm_hold(sm_t *sm, const uint64_t absolute_time) {
    if (sm->state != SM_STATE_COUNTDOWN && sm->state != SM_STATE_POLLING)
        return;

    sm->state = SM_STATE_HOLD;

    sm->hold_state = sm->state;
    sm->hold_time  = absolute_time;
}

void sm_continue_new_t0(sm_t *sm, uint64_t t0) {
    if (sm->state != SM_STATE_HOLD) return;

    sm_state_t continue_state = sm->hold_state;
    if (continue_state == SM_STATE_HOLD) {
        // this is very bad and should never happen
        continue_state = SM_STATE_COUNTDOWN;
    }

    sm->state = continue_state;
    sm->t0    = t0;
}

void sm_continue(sm_t *sm, const uint64_t absolute_time) {
    uint64_t time_at_continue = absolute_time;
    uint64_t time_held        = time_at_continue - sm->hold_time;

    uint64_t new_t0 = sm->t0 + time_held;

    sm_continue_new_t0(sm, new_t0);
}

void sm_continue_old_t0(sm_t *sm) {
    sm_continue_new_t0(sm, sm->t0);
}

int64_t sm_relative_time(sm_t *sm, const uint64_t absolute_time) {
    if (sm->state == SM_STATE_HOLD) {
        // countdown stays static during hold
        // it's held at the time at which it was held

        return sm->hold_time - sm->t0;
    }

    return absolute_time - (int64_t)sm->t0;
}

void sm_poll_answer(sm_t *sm, sm_poll_status_t status,
                    const uint64_t absolute_time) {
    // already gone through all polls
    if (sm->current_poll >= sm->num_polls) return;

    const sm_poll_t *poll = &sm->polls[sm->current_poll];

    if (sm->state != SM_STATE_POLLING) {
        // not polling or holding, so return
        if (sm->state != SM_STATE_HOLD) return;

        // holding, so only answer a poll if one's pending
        if (sm_relative_time(sm, absolute_time) < poll->start_time) {
            // we're not in a poll, return
            return;
        }
    }

    sm->current_poll_status = status;
}

void sm_run_polls_events(sm_t *sm, const uint64_t absolute_time) {
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

            sm_hold(sm, absolute_time);
            return;
        }

        if (sm_relative_time(sm, absolute_time) >
            poll->start_time + poll->timeout_duration) {
            // poll is INIT + timed out, hold countdown

            sm_hold(sm, absolute_time);
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

            if (sm_relative_time(sm, absolute_time) >= poll->start_time) {
                // we should be in a poll, switch state
                sm->state = SM_STATE_POLLING;
                return;
            }
        }

        // check if event
        if (sm->upcoming_event < sm->num_events) {
            const sm_event_t *event = &sm->events[sm->upcoming_event];

            if (sm_relative_time(sm, absolute_time) >= event->time) {
                // run the event
                event->fn();

                // move on to next event
                sm->upcoming_event++;
                return;
            }
        }

        if (sm_relative_time(sm, absolute_time) >= 0) {
            // countdown is over, go to flight

            sm->state = SM_STATE_FLIGHT;
            return;
        }
    }
}
