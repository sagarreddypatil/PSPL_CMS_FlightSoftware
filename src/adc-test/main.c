#include <pico/stdlib.h>
#include <stdio.h>
#include <ads13x.h>
#include <max31856.h>
#include <w25n01.h>

#define ADC_RST 7

myspi_device_t tc_0;
myspi_device_t tc_1;
myspi_device_t ads13x;
myspi_device_t flash;

// SPI_DEVICE(tc_0, spi0, 1);
// SPI_DEVICE(tc_1, spi0, 0);
// SPI_DEVICE(ads13x, spi0, 6);
// SPI_DEVICE(flash, spi0, 20);

static const int32_t ADC_MAX = 0x7FFFFF;
static const int32_t ADC_MIN = -0x800000;

int main() {
    gpio_init(ADC_RST);
    gpio_set_dir(ADC_RST, GPIO_OUT);
    gpio_put(ADC_RST, 1);
    stdio_init_all();
    while (!stdio_usb_connected()) tight_loop_contents();

    // for (int i = 0; i < 15; i++) {
    //   printf("iter %d, time %llu\n", i, time_us_64());
    // }
    // printf("\n");

    // while (true)
    //   ;

    for (int i = 0; i < 50; i++) printf("\n");

    printf("Program: %s\n", PICO_PROGRAM_NAME);
    printf("Version: %s\n", PICO_PROGRAM_VERSION_STRING);
    printf("\n=====Startup Information=====\n");
    printf("Took %llu µs\n", time_us_64());

    myspi_device_init(&tc_0, myspi0, 1, 2, 3, 4, SPI_CPOL_0, SPI_CPHA_1,
                      5000000);
    myspi_device_init(&tc_1, myspi0, 0, 2, 3, 4, SPI_CPOL_0, SPI_CPHA_1,
                      5000000);
    myspi_device_init(&ads13x, myspi0, 6, 2, 3, 4, SPI_CPOL_0, SPI_CPHA_1,
                      15000000);
    myspi_device_init(&ads13x, myspi0, 29, 2, 3, 4, SPI_CPOL_0, SPI_CPHA_1,
                      15000000);

    printf("Initializing ADS13x...\n");
    uint baud = myspi_configure(&ads13x);
    ads13x_reset(&ads13x);
    while (!ads13x_ready(&ads13x)) tight_loop_contents();
    printf("ADS13x ready!\n");
    ads13x_init(&ads13x);
    printf("ADS13x initialized!\n");
    printf("ADC Baud: %u\n", baud);

    printf("chan1,chan2\n");
    while (true) {
        // // print all regs
        // uint16_t id     = ads13x_rreg_single(ads13x, ads13x_id);
        // uint16_t status = ads13x_rreg_single(ads13x, ads13x_status);
        // uint16_t mode   = ads13x_rreg_single(ads13x, ads13x_mode);
        // uint16_t clock  = ads13x_rreg_single(ads13x, ads13x_clock);
        // uint16_t gain   = ads13x_rreg_single(ads13x, ads13x_gain);
        // uint16_t cfg    = ads13x_rreg_single(ads13x, ads13x_cfg);

        // printf("ID:     %04x\n", id);
        // printf("Status: %04x\n", status);
        // printf("Mode:   %04x\n", mode);
        // printf("Clock:  %04x\n", clock);
        // printf("Gain:   %04x\n", gain);
        // printf("Cfg:    %04x\n", cfg);

        // sleep_ms(100);

        uint16_t statA;
        // uint16_t statB;
        int32_t data[2];
        ads13x_read_data(&ads13x, &statA, data, 2);

        float fdata[2] = {1.2 * data[0] / (float)ADC_MAX,
                          1.2 * data[1] / (float)ADC_MAX};
        // printf("Stat: %04x, A: %ld, B: %ld\n", statA, data[0], data[1]);
        // printf("%ld, %ld\n", data[0], data[1]);
        printf("%f,%f\n", fdata[0], fdata[1]);
    }
}
