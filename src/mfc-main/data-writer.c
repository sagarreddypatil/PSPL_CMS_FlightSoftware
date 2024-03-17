#include "mfc.h"
#include <mydma.h>

#define UDP_PACKET_SIZE 1024

void data_writer_main() {

    myspi_lock(&eth0);
    w5500_create_udp_socket(&eth0, SENSORNET_SOCKET, SENSORNET_SRC_PORT, false,
                            false, false); // TODO: not sure if port 0 is valid
    w5500_write(&eth0, SENSORNET_SOCKET, W5500_Sn_DIPR0, SENSORNET_IP, sizeof(SENSORNET_IP));
    w5500_write16(&eth0, SENSORNET_SOCKET, W5500_Sn_DPORT0,
                  SENSORNET_DEST_PORT);
    myspi_unlock(&eth0);

    uint8_t send_buf[1024];
    size_t offset_bytes = 0;

    TickType_t time = xTaskGetTickCount();

    while (true) {
        sensornet_packet_t packet;

        if (offset_bytes <= UDP_PACKET_SIZE - sizeof(sensornet_packet_t)) {
            if (xQueueReceive(data_writer_queue, &packet, portMAX_DELAY) ==
                pdFALSE) {
                continue;
            }

            dmacpy(send_buf + offset_bytes, &packet, sizeof(sensornet_packet_t));
            offset_bytes += sizeof(sensornet_packet_t);
            continue;
        }
        myspi_lock(&eth0);
        // send the packet
        w5500_error_t status =
            w5500_write_data(&eth0, SENSORNET_SOCKET, &send_buf, offset_bytes);
        myspi_unlock(&eth0);

        if (status == W5500_SUCCESS) offset_bytes = 0;
        else {
            safeprintf("Failed, status = %d", status);
        }
        xTaskDelayUntil(&time, 2);
    }
}
