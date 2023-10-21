#include <ntp.h>
#include <stdio.h>

#define ntohl(x) __builtin_bswap32(x)

#define NTP_TIMESTAMP_DELTA \
  2208988800ull  // NTP starts in 1900 for some stupid reason

#define SEC_TO_US 1000000ull

#define NTP_PORT 123

static const uint64_t RESP_TIMEOUT = 5000;  // microseconds

uint64_t ntp_time_to_us(ntp_time_t ntp_time) {
  uint64_t time_us = (uint64_t)ntp_time.seconds * SEC_TO_US +
                     (uint64_t)ntp_time.fraction * SEC_TO_US / UINT32_MAX;
  time_us = time_us - (NTP_TIMESTAMP_DELTA * SEC_TO_US);
  return time_us;
}

int64_t get_server_time(SPI_DEVICE_PARAM, ip_t server_addr,
                        w5500_socket_t socket) {
  ntp_packet_t packet = {0};

  packet.li_vn_mode = 0b00011011;  // li=0, vn=3, mode=3, DEC 27

  w5500_create_udp_socket(spi, socket, NTP_PORT, false, false, false);
  w5500_write(spi, socket, W5500_Sn_DIPR0, server_addr, 4);
  w5500_write16(spi, socket, W5500_Sn_DPORT0, NTP_PORT);

  w5500_write_data(spi, socket, &packet, sizeof(ntp_packet_t));
  uint64_t t0 = time_us_64();  // t0

  int read    = 0;
  uint64_t t3 = 0;

  uint64_t timeout_start = time_us_64();
  while ((read = w5500_read_data(spi, socket, (uint8_t*)&packet,
                                 sizeof(ntp_packet_t))) == 0) {
    tight_loop_contents();
    t3 = time_us_64();
    if (time_us_64() - timeout_start > RESP_TIMEOUT) {
      break;
    }
  }

  if (read != sizeof(ntp_packet_t)) {
    return INT64_MIN;  // error value
  }

  packet.rec.seconds  = ntohl(packet.rec.seconds);
  packet.rec.fraction = ntohl(packet.rec.fraction);

  packet.xmt.seconds  = ntohl(packet.xmt.seconds);
  packet.xmt.fraction = ntohl(packet.xmt.fraction);

  uint64_t t1 = ntp_time_to_us(packet.rec);
  uint64_t t2 = ntp_time_to_us(packet.xmt);
 
  uint64_t server_time_center          = (t1 + t2) / 2;
  uint64_t local_time_at_server_center = (t0 + t3) / 2;

  int64_t temp_offset = t2 - t3; //While t1 is not working, use this calculation 
  int64_t offset = server_time_center - local_time_at_server_center;

  printf("t0:  %lld\n", t0);
  printf("t1: %lld\n", t1); //t1 receives a 0 input, so returns the - conversion from the year 1900
  printf("t2: %lld\n", t2); //t2 returns the proper number (current time)
  printf("t3: %lld\n", t3); //t0 and t3 return proper numbers (system time since start, but called at different times)
  //printf("%lld  -  ", t1 + t2);
  //printf("%lld  = 2*  ", t0 + t3);

  return temp_offset;
}

