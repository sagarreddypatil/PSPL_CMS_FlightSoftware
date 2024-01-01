#include "emu.h"
#include <sensornet.h>
#include <hardware/adc.h>

ip_t recv_ip       = {192, 168, 2, 20};
uint16_t recv_port = 5001;

void sensornet_task_init() {
  w5500_create_udp_socket(&w5500, W5500_S1, 5001, false, false, false);

  adc_init();
  adc_gpio_init(PYRO_CONT_0);
  adc_gpio_init(PYRO_CONT_1);

  w5500_write(&w5500, W5500_S1, W5500_Sn_DIPR0, recv_ip, 4);
  w5500_write16(&w5500, W5500_S1, W5500_Sn_DPORT0, recv_port);
}

const uint64_t epoch =
    1693108800000000;  // Aug 27, 2023 00:00:00 UTC, beacuse NTP not here yet

uint64_t pyroLastSampleTime       = epoch;
const uint64_t PYRO_SAMPLE_PERIOD = 10000;  // 100 hz

uint64_t adcLastSampleTime       = epoch;
const uint64_t ADC_SAMPLE_PERIOD = 10000;  // 100 hz

void sensornet_task_run() {
  const uint64_t now = time_us_64() + time_offset;

  if (now - adcLastSampleTime > ADC_SAMPLE_PERIOD) {
    uint16_t status;
    int32_t data[2];

    static uint64_t counter0 = 0;
    static uint64_t counter1 = 0;

    ads13x_read_data(&ads13x, &status, data, 2);

    sensornet_packet_t packet_adc_0 = {.type    = "SEN",
                                       .id      = 3,
                                       .time_us = now,
                                       .counter = counter0++,
                                       .value   = data[0]};

    sensornet_packet_t packet_adc_1 = {.type    = "SEN",
                                       .id      = 4,
                                       .time_us = now,
                                       .counter = counter1++,
                                       .value   = data[1]};

    printf("time:\t%lld\tstatus:%x\tadc_0:\t%ld\tadc_1:\t%ld\n", now, status,
           data[0], data[1]);

    w5500_write_data(&w5500, W5500_S1, &packet_adc_0,
                     sizeof(sensornet_packet_t));
    w5500_write_data(&w5500, W5500_S1, &packet_adc_1,
                     sizeof(sensornet_packet_t));

    adcLastSampleTime = now;
  }

  if (now - pyroLastSampleTime > PYRO_SAMPLE_PERIOD) {
    pyroLastSampleTime = now;

    adc_select_input(PYRO_CONT_0_INPUT);
    uint16_t pyro_cont_0 = adc_read();

    adc_select_input(PYRO_CONT_1_INPUT);
    uint16_t pyro_cont_1 = adc_read();

    static uint64_t counterCont0 = 0;
    static uint64_t counterCont1 = 0;

    sensornet_packet_t packet_cont_0 = {.type    = "SEN",
                                        .id      = 1,
                                        .time_us = now,
                                        .counter = counterCont0++,
                                        .value   = pyro_cont_0};

    sensornet_packet_t packet_cont_1 = {.type    = "SEN",
                                        .id      = 2,
                                        .time_us = now,
                                        .counter = counterCont1++,
                                        .value   = pyro_cont_1};

    // printf("time:\t%lld\tpyro_cont_0:\t%d\tpyro_cont_1:\t%d\n", now,
    //        pyro_cont_0, pyro_cont_1);

    w5500_write_data(&w5500, W5500_S1, &packet_cont_0,
                     sizeof(sensornet_packet_t));

    w5500_write_data(&w5500, W5500_S1, &packet_cont_1,
                     sizeof(sensornet_packet_t));
  }
}
