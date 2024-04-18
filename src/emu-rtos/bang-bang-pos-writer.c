#include <emu.h>
#include <hardware/gpio.h>

void bang_bang_writer_main() {

    TickType_t prev_wake = xTaskGetTickCount();

    uint64_t counter = 0;

    uint8_t valves[3] = {FUEL_SOLENOID, OX_SOLENOID, AUX_SOLENOID};

    while (true) {
        for (size_t i = 0; i < 3; i++) {        // for each 3 bang bang valves
            sensornet_packet_t packet = {
                .id      = SENSOR_ID_BB_VALVES_START + i,
                .counter = counter,
                .time_us = unix_time_us(),
                .value   = gpio_get(valves[i])
            };

            xQueueSend(data_writer_queue, &packet, 0);
        }
        counter++;
        
        vTaskDelayUntil(&prev_wake, pdMS_TO_TICKS(BB_LOOP_PERIOD*2));
    }
}