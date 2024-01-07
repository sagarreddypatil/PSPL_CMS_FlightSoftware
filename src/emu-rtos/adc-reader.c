#include "emu.h"
#include <ads13x.h>

#define CHANNELS_MASK ((1 << ADC0_CHANNELS) - 1)

bool adc0_init_routine() {
    // this exists in case ads13x_init returns a bool in the future
    // for better error handling, so that we can also run this in a delay
    // loop waiting for the ADS13x to start working in the case that
    // it doesn't

    safeprintf("Initializing ADC0\n");

    myspi_lock(&adc0);
    myspi_configure(&adc0);
    ads13x_reset(&adc0);
    myspi_unlock(&adc0);

    bool adc_ready   = false;
    TickType_t delay = 1;
    while (!adc_ready) {
        myspi_lock(&adc0);
        myspi_configure(&adc0);
        adc_ready = ads13x_ready(&adc0);
        myspi_unlock(&adc0);

        safeprintf("ADC not yet ready, delaying: %d\n", delay);
        if (!adc_ready) vTaskDelay(delay);
        delay *= 2;
    }

    safeprintf("ADC0 Ready\n");

    myspi_lock(&adc0);
    myspi_configure(&adc0);
    ads13x_init(&adc0);
    myspi_unlock(&adc0);

    return true;
}

void adc0_reader_main() {
    adc0_init_routine();
    safeprintf("ADC0 Initialized\n");

    uint64_t counter = 0;

    return;

    while (true) {
        // Wait on DRDY ISR
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        uint16_t status_reg;
        int32_t data[ADC0_CHANNELS];  // this is an ads131m06, 6 channel

        myspi_lock(&adc0);
        myspi_configure(&adc0);
        ads13x_read_data(&adc0, &status_reg, data, ADC0_CHANNELS);
        uint64_t sample_time = unix_time_us();
        myspi_unlock(&adc0);

        // checking if all channels are DRDY'd (they should be)
        if ((status_reg & CHANNELS_MASK) != CHANNELS_MASK) {
            safeprintf("ADC0: All channels not DRDYd!\n");
        }

        bool success = true;
        for (int i = 0; i < ADC0_CHANNELS; i++) {
            sensornet_packet_t packet = {
                .id      = SENSOR_ID_ADC0_START + i,
                .counter = counter,
                .time_us = sample_time,
                .value   = data[i],
            };

            // this CAN NOT take up time
            success &= xQueueSend(data_writer_queue, &packet, 0);
        }

        counter++;
    }
}