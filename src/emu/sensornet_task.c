#include "emu.h"
#include <sensornet.h>
#include <hardware/adc.h>

ip_t recv_ip       = {192, 168, 2, 10};
uint16_t recv_port = 5001;

void sensornet_task_init() {
  w5500_create_udp_socket(w5500, W5500_S1, 5001, false, false, false);

  adc_init();
  adc_gpio_init(PYRO_CONT_0);
  adc_gpio_init(PYRO_CONT_1);

  w5500_write(w5500, W5500_S0, W5500_Sn_DIPR0, recv_ip, 4);
  w5500_write16(w5500, W5500_S0, W5500_Sn_DPORT0, recv_port);
}

uint64_t lastSampleTime           = 0;
const uint64_t PYRO_SAMPLE_PERIOD = 10000;

uint64_t counter = 0;

void sensornet_task_run() {
  const uint64_t now = time_us_64();

  if (now - lastSampleTime > PYRO_SAMPLE_PERIOD) {
    lastSampleTime = now;

    adc_select_input(PYRO_CONT_0_INPUT);
    uint16_t pyro_cont_0 = adc_read();

    adc_select_input(PYRO_CONT_1_INPUT);
    uint16_t pyro_cont_1 = adc_read();

    sensornet_packet_t packet_cont_0 = {.type    = "SEN",
                                        .id      = 0,
                                        .time_us = now,
                                        .counter = counter,
                                        .value   = pyro_cont_0};

    sensornet_packet_t packet_cont_1 = {.type    = "SEN",
                                        .id      = 1,
                                        .time_us = now,
                                        .counter = counter + 1,
                                        .value   = pyro_cont_1};

    w5500_write_data(w5500, W5500_S0, &packet_cont_0,
                     sizeof(sizeof(sensornet_packet_t)));

    w5500_write_data(w5500, W5500_S0, &packet_cont_1,
                     sizeof(sizeof(sensornet_packet_t)));

    counter += 2;
  }
}