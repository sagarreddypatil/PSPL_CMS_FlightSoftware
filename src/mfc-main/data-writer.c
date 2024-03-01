#include "mfc.h"
#include <mydma.h>

#define UDP_PACKET_SIZE 1024

void data_writer_main() {
    myspi_lock(&eth0);
    w5500_create_udp_socket(&eth0, SENSORNET_SOCKET, SENSORNET_SRC_PORT, false,
                            false, false); // TODO: not sure if port 0 is valid
    w5500_write(&eth0, SENSORNET_SOCKET, W5500_Sn_DIPR0, SENSORNET_IP, 4);
    w5500_write16(&eth0, SENSORNET_SOCKET, W5500_Sn_DPORT0,
                  SENSORNET_DEST_PORT);
    myspi_unlock(&eth0);

    uint64_t total = 0;

    uint8_t send_buf[1024];
    size_t offset = 0;

    while (true) {
        sensornet_packet_t packet;

        if (offset <= UDP_PACKET_SIZE - sizeof(sensornet_packet_t)) {
            if (xQueueReceive(data_writer_queue, &packet, portMAX_DELAY) ==
                pdFALSE) {
                continue;
            }

            dmacpy(send_buf + offset, &packet, sizeof(sensornet_packet_t));
            offset += sizeof(sensornet_packet_t);
            total++;

            continue;
        }

        // send the packet
        myspi_lock(&eth0);
        w5500_error_t status =
            w5500_write_data(&eth0, SENSORNET_SOCKET, &send_buf, offset);
        myspi_unlock(&eth0);

        if (status == W5500_SUCCESS) offset = 0;
        else {
            safeprintf("Failed, status = %d", status);
        }
    }
}
