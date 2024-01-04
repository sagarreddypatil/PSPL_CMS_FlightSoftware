#include "emu.h"

void data_writer_task() {
    w5500_create_udp_socket(&w5500, SENSORNET_SOCKET, 5001, false, false,
                            false);

    w5500_write(&w5500, SENSORNET_SOCKET, W5500_Sn_DIPR0, DATA_RECV_IP, 4);
    w5500_write16(&w5500, SENSORNET_SOCKET, W5500_Sn_DPORT0, SENSORNET_PORT);

    while (true) {
        sensornet_packet_t packet;
        if (xQueueReceive(outputQueueHandle, &packet, portMAX_DELAY) ==
            pdFALSE) {
            continue;
        }

        // send the packet
        w5500_error_t status = w5500_write_data(
            &w5500, SENSORNET_SOCKET, &packet, sizeof(sensornet_packet_t));

        if (status < 0) {
            // insufficient space in socket, put the packet back in the queue
            xQueueSendToFront(outputQueueHandle, &packet, portMAX_DELAY);
        }
    }
}