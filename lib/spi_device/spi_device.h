#ifndef SPI_DEVICE_H
#define SPI_DEVICE_H

#include <pico/stdlib.h>
#include <hardware/spi.h>

#define SPI_DEVICE(name, max_cmd_len)              \
    void name##_set(spi_inst_t *spi);              \
    const size_t name##_max_cmd_len = max_cmd_len; \
    uint8_t name##_cmd_buf[max_cmd_len];

// VA_ARGS is the parameter list
#define SPI_DEVICE_COMMAND(name, cmd_name, ...) \
    void name##_##cmd_name(spi_inst_t *spi, __VA_ARGS__);

#endif // SPI_DEVICE_H