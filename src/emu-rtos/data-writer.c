#include "emu.h"

void data_writer_main() {
    myspi_lock(&eth0);
    w5500_create_udp_socket(&eth0, SENSORNET_SOCKET, SENSORNET_SRC_PORT, false,
                            false, false);
    w5500_write(&eth0, SENSORNET_SOCKET, W5500_Sn_DIPR0, SENSORNET_IP, 4);
    w5500_write16(&eth0, SENSORNET_SOCKET, W5500_Sn_DPORT0,
                  SENSORNET_DEST_PORT);
    myspi_unlock(&eth0);

    while (true) {
        sensornet_packet_t packet;
        if (xQueueReceive(data_writer_queue, &packet, portMAX_DELAY) ==
            pdFALSE) {
            continue;
        }

        // send the packet
        myspi_lock(&eth0);
        w5500_error_t status = w5500_write_data(
            &eth0, SENSORNET_SOCKET, &packet, sizeof(sensornet_packet_t));
        myspi_unlock(&eth0);

        if (status < 0) {
            // insufficient space in socket, put the packet back in the queue
            xQueueSendToFront(data_writer_queue, &packet, portMAX_DELAY);
        }
    }
}