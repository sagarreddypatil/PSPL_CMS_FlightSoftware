#include "mfc.h"
#include <mydma.h>
#include <pico/unique_id.h>

#define UDP_PACKET_SIZE 1024

int64_t offset = 0;
bool ntp_sync() {
    // TODO: make something that'll work in flight

    int64_t new_offset = get_server_time(&eth0, NTP_SERVER_IP, NTP_SOCKET);

    if (new_offset > 0) {
        offset = new_offset;
        return true;
    }
    return false;
}

bool eth0_init() {

    myspi_lock(&eth0);      // only need to do this once, as it's the only
    myspi_configure(&eth0); // device on this bus 

    // Acquire and set mac address
    pico_unique_board_id_t board_id;
    pico_get_unique_board_id(&board_id);

    const mac_t src_mac = {0x02,           board_id.id[3], board_id.id[4],
                           board_id.id[5], board_id.id[6], board_id.id[7]};

    w5500_reset(&eth0);
    while (!w5500_ready(&eth0)) tight_loop_contents();  // TODO: timeout necessary

#ifndef NDEBUG
    uint64_t start = time_us_64();
#endif
    uint delay     = 100;

    while (!w5500_has_link(&eth0)) {  // TODO: timeout necessary
        delay += 1;
        sleep_ms(delay);
    }

#ifndef NDEBUG
    safeprintf("W5500 has link, took %d us\n",
               (int)(time_us_64() - start));
#endif

    // Configures W5500 with info
    ip_t ip;
    w5500_config(&eth0, src_mac, SRC_IP, SUBNET_MASK, GATEWAY_IP);
    w5500_read(&eth0, W5500_COMMON, W5500_SIPR0, ip, sizeof(ip));

    safeprintf("Ethernet Connected, IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2],
               ip[3]);
    
    if (ntp_sync()) {
        safeprintf("Time synced, offset: %lld\n", offset);
    } else {
        safeprintf("NTP Sync Failed! Halting, offset: %lld\n", offset);
        while (1) tight_loop_contents();
    }
    
    return true;
}

void data_writer_main() {

    if (!eth0_init()) {
        safeprintf("Ethernet failed to connect!");
        while (true) tight_loop_contents();
    }

    w5500_create_udp_socket(&eth0, SENSORNET_SOCKET, SENSORNET_SRC_PORT, false,
                            false, false); // TODO: not sure if port 0 is valid
    w5500_write(&eth0, SENSORNET_SOCKET, W5500_Sn_DIPR0, SENSORNET_IP, 4);
    w5500_write16(&eth0, SENSORNET_SOCKET, W5500_Sn_DPORT0,
                  SENSORNET_DEST_PORT);

    uint8_t send_buf[1024];
    size_t offset_bytes = 0;

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

        // send the packet
        w5500_error_t status =
            w5500_write_data(&eth0, SENSORNET_SOCKET, &send_buf, offset_bytes);

        if (status == W5500_SUCCESS) offset_bytes = 0;
        else {
            safeprintf("Failed, status = %d", status);
        }
    }
}