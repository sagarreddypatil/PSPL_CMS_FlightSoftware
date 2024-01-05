#include "emu.h"

int64_t offset = 0;

static inline uint64_t unix_time_us() {
    return time_us_64() + offset;
}

void ntp_test_main() {
    while (true) {
        myspi_lock(&eth0);
        int64_t new_offset = get_server_time(&eth0, NTP_SERVER_IP, NTP_SOCKET);
        myspi_unlock(&eth0);

        if (new_offset > 0) {
            offset = new_offset;
            safeprintf("%lld,%lld\n", unix_time_us(), offset);
        } else {
            safeprintf("NTP got error/timed out\n");
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}