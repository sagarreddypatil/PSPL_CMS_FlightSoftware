#include "emu.h"
#include "state-machine.h"

void sm_task_main() {
    TickType_t prev_wake    = xTaskGetTickCount();
    uint64_t packet_counter = 0;

    while (true) {
        // solve the state machine and get the current T-0 time
        global_lock();
        sm_run_polls_events(&state_machine);
        const int64_t relative_time = sm_relative_time(&state_machine);
        global_unlock();

        // post the time to SensorNet, low latency so don't use the buffer
        sensornet_packet_t packet = {.id      = SENSOR_ID_VEHICLE_CLOCK,
                                     .counter = packet_counter++,
                                     .time_us = sm_absolute_time(),
                                     .value   = relative_time};

        safeprintf("Relative Time: %" PRId64 "\n", relative_time);

        myspi_lock(&eth0);
        w5500_error_t status = w5500_write_data(
            &eth0, SENSORNET_SOCKET, &packet, sizeof(sensornet_packet_t));
        myspi_unlock(&eth0);

        xTaskDelayUntil(&prev_wake, pdMS_TO_TICKS(STATE_MACHINE_TICK));
    }
}
