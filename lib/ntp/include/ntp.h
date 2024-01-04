#pragma once

#include <stdint.h>
#include <w5500.h>
#include <myspi.h>

/*
 * Returns offset between server and local time
 */
int64_t get_server_time(SPI_DEVICE_PARAM, ip_t server_addr,
                        w5500_socket_t socket);

// https://github.com/ntp-project/ntp/blob/master-no-authorname/include/ntp.h#L535
// https://github.com/ntp-project/ntp/blob/master-no-authorname/include/ntp_fp.h#L36
// REFERENCE COPYRIGHT LATER

typedef struct {
    uint32_t seconds;
    uint32_t fraction;
} ntp_time_t;

typedef struct {
    uint8_t li_vn_mode; /* peer leap indicator */
    uint8_t stratum;    /* peer stratum */
    uint8_t ppoll;      /* peer poll interval */
    int8_t precision;   /* peer clock precision */
    uint32_t rootdelay; /* roundtrip delay to primary source */
    uint32_t rootdisp;  /* dispersion to primary source*/
    uint32_t refid;     /* reference id */
    ntp_time_t reftime; /* last update time */
    ntp_time_t org;     /* originate time stamp (t0) */
    ntp_time_t rec;     /* receive time stamp (t1) */
    ntp_time_t xmt;     /* transmit time stamp (t2) */
} ntp_packet_t;
