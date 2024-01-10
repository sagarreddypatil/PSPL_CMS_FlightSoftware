#include "emu.h"
#include <mydma.h>

#define UDP_PACKET_SIZE 1024

void data_writer_main() {
    myspi_lock(&eth0);
    w5500_create_udp_socket(&eth0, SENSORNET_SOCKET, SENSORNET_SRC_PORT, false,
                            false, false);
    w5500_write(&eth0, SENSORNET_SOCKET, W5500_Sn_DIPR0, SENSORNET_IP, 4);
    w5500_write16(&eth0, SENSORNET_SOCKET, W5500_Sn_DPORT0,
                  SENSORNET_DEST_PORT);
    myspi_unlock(&eth0);

    uint8_t send_buf[1024];
    size_t offset = 0;

    while (true) {
        sensornet_packet_t packet;
        if (xQueueReceive(data_writer_queue, &packet, portMAX_DELAY) ==
            pdFALSE) {
            continue;
        }

        // print the packet for testing
        safeprintf("ID: %-6" PRIu16 "| Counter: %-8" PRIu64
                   "| Time: %-10" PRIu64 "| Value: %" PRId64 "\n",
                   packet.id, packet.counter, packet.time_us, packet.value);
        safeprintf("Queue Space: %d\n",
                   uxQueueSpacesAvailable(data_writer_queue));

        if (offset < UDP_PACKET_SIZE - sizeof(sensornet_packet_t) - 1) {
            dmacpy(send_buf + offset, &packet, sizeof(sensornet_packet_t));
            offset += sizeof(sensornet_packet_t);
            continue;
        }

        // send the packet
        myspi_lock(&eth0);
        w5500_error_t status =
            w5500_write_data(&eth0, SENSORNET_SOCKET, &send_buf, offset);
        myspi_unlock(&eth0);

        if (status == W5500_SUCCESS) offset = 0;

        // if (status < 0) {
        //     // insufficient space in socket, put the packet back in the queue
        //     xQueueSendToFront(data_writer_queue, &packet, portMAX_DELAY);
        // }
    }
}