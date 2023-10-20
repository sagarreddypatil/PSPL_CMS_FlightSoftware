#include <stdint.h>
#include <w5500.h>

/*
 * Returns offset between server and local time
 */
int64_t get_server_time(SPI_DEVICE_PARAM, ip_t server_addr,
                        w5500_socket_t socket);

// https://github.com/ntp-project/ntp/blob/master-no-authorname/include/ntp.h#L535
// https://github.com/ntp-project/ntp/blob/master-no-authorname/include/ntp_fp.h#L36
// REFERENCE COPYRIGHT LATER

typedef int32 s_fp;
typedef u_int32 u_fp;
typedef struct {
  union {
    u_int32 Xl_ui;
    int32 Xl_i;
  } Ul_i;
  u_int32 l_uf;
} l_fp;

struct pkt {
  u_char li_vn_mode; /* peer leap indicator */
  u_char stratum;    /* peer stratum */
  u_char ppoll;      /* peer poll interval */
  s_char precision;  /* peer clock precision */
  u_fp rootdelay;    /* roundtrip delay to primary source */
  u_fp rootdisp;     /* dispersion to primary source*/
  u_int32 refid;     /* reference id */
  l_fp reftime;      /* last update time */
  l_fp org;          /* originate time stamp */
  l_fp rec;          /* receive time stamp */
  l_fp xmt;          /* transmit time stamp */
}
