#include <stdint.h>
#include <w5500.h>

/*
 * Portions of this program come from
 * https://github.com/lettier/ntpclient/blob/master/source/c/main.c
 */

/*
BSD 3-Clause License

Copyright (c) 2014, David Lettier
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*
*/

typedef struct {
  uint8_t li_vn_mode;  // Eight bits. li, vn, and mode.
                       // li.   Two bits.   Leap indicator.
                       // vn.   Three bits. Version number of the protocol.
                       // mode. Three bits. Client will pick mode 3 for client.

  uint8_t stratum;  // Eight bits. Stratum level of the local clock.
  uint8_t poll;     // Eight bits. Maximum interval between successive messages.
  uint8_t precision;  // Eight bits. Precision of the local clock.

  uint32_t rootDelay;  // 32 bits. Total round trip delay time.
  uint32_t
      rootDispersion;  // 32 bits. Max error aloud from primary clock source.
  uint32_t refId;      // 32 bits. Reference clock identifier.

  uint32_t refTm_s;  // 32 bits. Reference time-stamp seconds.
  uint32_t refTm_f;  // 32 bits. Reference time-stamp fraction of a second.

  uint32_t origTm_s;  // 32 bits. Originate time-stamp seconds.
  uint32_t origTm_f;  // 32 bits. Originate time-stamp fraction of a second.

  uint32_t rxTm_s;  // 32 bits. Received time-stamp seconds.
  uint32_t rxTm_f;  // 32 bits. Received time-stamp fraction of a second.

  uint32_t txTm_s;  // 32 bits and the most important field the client cares
                    // about. Transmit time-stamp seconds.
  uint32_t txTm_f;  // 32 bits. Transmit time-stamp fraction of a second.
} ntp_packet_t;

typedef struct {
  uint64_t local_us;
  uint64_t server_us;
} ntp_resp_t;

ntp_resp_t get_server_time(SPI_DEVICE_PARAM, ip_t server_addr,
                           w5500_socket_t socket);

void run_ntp_test();