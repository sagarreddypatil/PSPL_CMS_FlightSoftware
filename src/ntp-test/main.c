#include <pico/stdlib.h>
#include <stdio.h>

#include <w5500.h>
#include <ntp.h>

#include <psp.h>

int64_t offset = 0;

myspi_device_t w5500;

static inline uint64_t unix_time_us() {
    return time_us_64() + offset;
}

int main() {
    stdio_init_all();
    while (!stdio_usb_connected())
        ;
    stdio_flush();

    myspi_bus_init(myspi1, 27, 26, 28);
    myspi_device_init(&w5500, myspi1, 25, 1, 1, 30000000);
    uint actual_baud = myspi_configure(&w5500);

    for (int i = 0; i < 10; i++) {
        printf("\n");
    }

    printf(psp_logo);
    printf("Program: %s\n", PICO_PROGRAM_NAME);
    printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);

    printf("actual baud: %d\n", actual_baud);

    ip_t gateway       = {192, 168, 2, 1};
    ip_t ntp_server_ip = {192, 168, 2, 1};
    ip_t subnet_mask   = {255, 255, 255, 0};
    ip_t src_ip        = {192, 168, 2, 50};
    mac_t src_mac      = {0x09, 0xA, 0xB, 0xC, 0xD, 0xE};

    w5500_reset(&w5500);
    uint64_t start = time_us_64();
    printf("Reset W5500, waiting for ready\n");
    while (!w5500_ready(&w5500))
        ;
    printf("W5500 ready, awaiting link, took %d us\n",
           (int)(time_us_64() - start));

    while (!w5500_has_link(&w5500))
        ;
    printf("W5500 has link, took %d us\n", (int)(time_us_64() - start));

    w5500_config(&w5500, src_mac, src_ip, subnet_mask, gateway);

    while (true) {
        int64_t new_offset = get_server_time(&w5500, ntp_server_ip, W5500_S3);
        if (new_offset > 0) {
            offset = new_offset;
        }

        printf("%lld,%lld\n", unix_time_us(), offset);

        sleep_ms(100);
    }
}
