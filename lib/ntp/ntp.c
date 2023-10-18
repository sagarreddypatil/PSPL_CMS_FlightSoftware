#include <ntp.h>
#include <stdio.h>

#define ntohl(x) __builtin_bswap32(x)

#define NTP_TIMESTAMP_DELTA \
  2208988800ull  // NTP starts in 1900 for some stupid reason

#define SEC_TO_US 1000000ull

#define NTP_PORT 123

static const uint64_t RESP_TIMEOUT = 10000;  // microseconds

ntp_resp_t get_server_time(SPI_DEVICE_PARAM, ip_t server_addr,
                           w5500_socket_t socket) {
  ntp_packet_t packet = {0};

  packet.li_vn_mode = 0b00011011;  // li=0, vn=3, mode=3, DEC 27

  w5500_create_udp_socket(spi, socket, NTP_PORT, false, false, false);
  w5500_write(spi, socket, W5500_Sn_DIPR0, server_addr, 4);
  w5500_write16(spi, socket, W5500_Sn_DPORT0, NTP_PORT);

  w5500_write_data(spi, socket, &packet, sizeof(ntp_packet_t));
  uint64_t write_time = time_us_64();  // t0

  int read           = 0;
  uint64_t recv_time = 0;

  uint64_t timeout_start = time_us_64();
  while ((read = w5500_read_data(spi, socket, (uint8_t*)&packet,
                                 sizeof(ntp_packet_t))) == 0) {
    tight_loop_contents();
    if (time_us_64() - timeout_start > RESP_TIMEOUT) {
      break;
    }
  }
  recv_time = time_us_64();  // t3

  if (read != sizeof(ntp_packet_t)) {
    printf("Error reading NTP packet\n");
    return (ntp_resp_t){0};
  }

  packet.txTm_s = ntohl(packet.txTm_s);  // Time-stamp seconds.
  packet.txTm_f = ntohl(packet.txTm_f);  // Time-stamp fraction of a second.

  uint32_t txTm = (packet.txTm_s - NTP_TIMESTAMP_DELTA);

  uint64_t server_time =
      (uint64_t)txTm * SEC_TO_US +
      (uint64_t)packet.txTm_f * SEC_TO_US / UINT32_MAX;  // i hate ntp now

  // rough approximation where the server time is the average of the write and
  // recieve times

  const uint64_t local_time_at_server_time =
      (write_time + recv_time) / 2;  // this is our time at the server time

  return (ntp_resp_t){.local_us  = local_time_at_server_time,
                      .server_us = server_time};
}