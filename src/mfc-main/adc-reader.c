#include "mfc.h"
#include <ads13x.h>
#include <hardware/gpio.h>

#define CHANNELS_MASK ((1 << ADC0_CHANNELS) - 1)

bool adc0_init() {
    // this exists in case ads13x_init returns a bool in the future
    // for better error handling, so that we can also run this in a delay
    // loop waiting for the ADS13x to start working in the case that
    // it doesn't

    // The chip takes 250 microseconds to start accepting commands after first
    // powered on. Not accounting for this causes this type of error:
    // You power on avionics, ADC init fails. You soft reboot, and now it works
    sleep_us(250);

    myspi_lock(&adc0);
    myspi_configure(&adc0);
    ads13x_reset(&adc0);
    myspi_unlock(&adc0);

    bool adc_ready = false;
    uint64_t start = time_us_64();

    while (!adc_ready) {
        myspi_lock(&adc0);
        myspi_configure(&adc0);
        adc_ready = ads13x_ready(&adc0);
        myspi_unlock(&adc0);
        if (time_us_64() - start > ADC0_READY_TIMEOUT) return false;
    }
    myspi_lock(&adc0);
    myspi_configure(&adc0);
    bool success = ads13x_init(&adc0);
    myspi_unlock(&adc0);
    if (!success) return false; // this line almost made me kms

    // Test voltage
    myspi_lock(&adc0);
    myspi_configure(&adc0);
    // ads13x_wreg_single(&adc0, 0x13, 0b10);
    // ads13x_wreg_single(&adc0, 0x18, 0b10);
    myspi_unlock(&adc0);

    // Channel 2 offset calibration
    myspi_lock(&adc0);
    myspi_configure(&adc0);
    ads13x_wreg_single(&adc0, 0x14, (0b111111110100000011011011 >> 8));
    ads13x_wreg_single(&adc0, 0x15, (0b111111110100000011011011 << 8) & 0xFF00);
    myspi_unlock(&adc0);

    safeprintf("\n\n%x\n\n", ads13x_rreg_single(&adc0, 0x13)); // TODO: this does not appear to be printing the expected value (0b10)

    return true;
}

void adc0_reader_main() {
    while (!adc0_init()) tight_loop_contents();

    gpio_set_irq_enabled_with_callback(ADC0_DRDY, GPIO_IRQ_EDGE_FALL, true,
                                       &adc0_drdy_isr);

    safeprintf("ADC0 Initialized\n");

    uint64_t counter   = 0;
    uint64_t prev_time = 0;

    // to safeguard against floating DRDY, for example
    uint64_t min_period = (1000000 / ADC0_RATE) / 2;  // div by 2 just in case

    while (true) {
        // Wait on DRDY ISR
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (time_us_64() - prev_time < min_period) {
            // something's wrong, shouldn't DRDY this fast
            continue;
        }
        prev_time = time_us_64();

        uint16_t status_reg;
        int32_t data[ADC0_CHANNELS];  // this is an ads131m06, 6 channel

        myspi_lock(&adc0);
        myspi_configure(&adc0);
        ads13x_read_data(&adc0, &status_reg, data, ADC0_CHANNELS);
        uint64_t sample_time = unix_time_us();
        myspi_unlock(&adc0);

        // checking if all channels are DRDY'd (they should be)
        uint16_t read_channels = status_reg & CHANNELS_MASK;
        // if ((read_channels) != CHANNELS_MASK) {
        //     safeprintf("ADC0: All channels not DRDYd!\n");
        // }

        for (int i = 0; i < ADC0_CHANNELS; i++) {
            // this channel did not DRDY, skip
            if (!(read_channels & (1 << i))) continue;

            // Check if data is greater than 24 bits
            if (data[i] > 0xFFFFFF) continue;

            sensornet_packet_t packet = {
                .id      = SENSOR_ID_ADC0_START + i,
                .counter = counter,
                .time_us = sample_time,
                .value   = data[i]
            };
            // safeprintf("%02x %02x\n", data[2], data[3]);

            // this CAN NOT take up time
            bool success = xQueueSend(data_writer_queue, &packet, 0);
        }
        counter++;
    }
}

void adc_drdy_isr(uint pin) {
    portDISABLE_INTERRUPTS();
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    vTaskNotifyGiveFromISR(adc0_reader_task, &xHigherPriorityTaskWoken);

    gpio_acknowledge_irq(pin, GPIO_IRQ_EDGE_FALL);

    portENABLE_INTERRUPTS();
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void adc0_drdy_isr() {
    adc_drdy_isr(ADC0_DRDY);
}
