#include "emu.h"
#include "config.h"
#include <hardware/adc.h>

void pyro_cont_reader_main() {
    uint64_t packet_counter = 0;
    TickType_t prev_wake = xTaskGetTickCount();

    while (true) {
        for (int i = 0; i < PYRO_CONTS; i++) {
            pyro_cont_mux_put(map_pyro_to_mux[i]);
            sleep_us(3); // ADC is 500 ksps

            uint16_t result = adc_read();
            uint64_t sample_time = unix_time_us();

            sensornet_packet_t packet = {
                .id = SENSOR_ID_PYRO_CONT_START + i,
                .counter = packet_counter,
                .time_us = sample_time,
                .value = result
            };

            xQueueSend(data_writer_queue, &packet, 0);
        }

        vTaskDelayUntil(&prev_wake, pdMS_TO_TICKS(PYRO_CONT_PERIOD));
        packet_counter++;
    }
}
