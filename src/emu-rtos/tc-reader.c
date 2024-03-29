#include "emu.h"

#include <max31856.h>
#include <assert.h>

void _tc_reader_main(int dev_index) {
    // slow enough, no need to DRDY

    TickType_t prev_wake = xTaskGetTickCount();

    myspi_device_t* device;
    sensornet_id_t probe_temp_id;
    sensornet_id_t cj_temp_id;

    static_assert(dev_index == 1 || dev_index == 0, "Parameter must be 0 or 1");

    if (dev_index == 0) {
        device        = &tc0;
        probe_temp_id = SENSOR_ID_TC0_PROBE_TEMP;
        cj_temp_id    = SENSOR_ID_TC0_CJ_TEMP;
    } else if (dev_index == 1) {
        device        = &tc1;
        probe_temp_id = SENSOR_ID_TC1_PROBE_TEMP;
        cj_temp_id    = SENSOR_ID_TC1_CJ_TEMP;
    }

    bool success = false;

    while (!success) {
        myspi_lock(device);
        myspi_configure(device);
        success = max31856_init(device);
        myspi_unlock(device);

        if (!success) vTaskDelay(pdMS_TO_TICKS(TC_INIT_FAIL_RETRY_TIME));
    }

    safeprintf("TC %d Initialized\n", dev_index);

    uint64_t counter = 0;

    while (true) {
        sensornet_packet_t probe_temp;
        sensornet_packet_t cj_temp;

        probe_temp.id = probe_temp_id;
        cj_temp.id    = cj_temp_id;

        probe_temp.counter = counter;
        cj_temp.counter    = counter;

        counter++;

        myspi_lock(device);
        myspi_configure(device);

        probe_temp.value   = max31856_get_tc_temp(device);
        probe_temp.time_us = unix_time_us();

        cj_temp.value   = max31856_get_cj_temp(device);
        cj_temp.time_us = unix_time_us();

        myspi_unlock(device);

        bool success = xQueueSend(data_writer_queue, &probe_temp,
                                  pdMS_TO_TICKS(TC_SAMPLE_PERIOD / 2));
        success      = success && xQueueSend(data_writer_queue, &cj_temp,
                                             pdMS_TO_TICKS(TC_SAMPLE_PERIOD / 2));

        if (!success) {
            safeprintf("TC%d packet did not send\n", dev_index);
        }

        vTaskDelayUntil(&prev_wake, pdMS_TO_TICKS(TC_SAMPLE_PERIOD));
    }
}

void tc0_reader_main() {
    _tc_reader_main(0);
}

void tc1_reader_main() {
    _tc_reader_main(1);
}