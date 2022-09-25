#ifndef SPI_DEVICE_IMPL_H
#define SPI_DEVICE_IMPL_H

#include <spi_device.h>
#include <pico/stdlib.h>
#include <hardware/spi.h>

#define SPI_MODE0                        \
    static spi_cpol_t cpol = SPI_CPOL_0; \
    static spi_cpha_t cpha = SPI_CPHA_0;

#define SPI_MODE1                        \
    static spi_cpol_t cpol = SPI_CPOL_0; \
    static spi_cpha_t cpha = SPI_CPHA_1;

#define SPI_MODE2                        \
    static spi_cpol_t cpol = SPI_CPOL_1; \
    static spi_cpha_t cpha = SPI_CPHA_0;

#define SPI_MODE3                        \
    static spi_cpol_t cpol = SPI_CPOL_1; \
    static spi_cpha_t cpha = SPI_CPHA_1;

#define SPI_DEVICE_IMPL(name, baud)                        \
    void name##_set(spi_inst_t *spi)                       \
    {                                                      \
        spi_set_baudrate(spi, baud);                       \
        spi_set_format(spi, 8, cpol, cpha, SPI_MSB_FIRST); \
    }                                                      \
    uint8_t name##_cmd_buf[name##_max_cmd_len];

#define SPI_DEVICE_COMMAND_IMPL_BEGIN(name, cmd_name, ...) \
    void name##_##cmd_name(__VA_ARGS__)                    \
    {

#define SPI_DEVICE_COMMAND_IMPL_INPUT(name, position) \
    memcpy(name##_cmd_buf + position, &name, sizeof(name));

#define SPI_DEVICE_COMMAND_IMPL_INPUT_CONST(val, position) \
    name##_cmd_buf[position] = val;

#define SPI_DEVICE_COMMAND_IMPL_EXEC() \
    spi_write_read_blocking(spi, name##_cmd_buf, NULL, name##_max_cmd_len);

#define SPI_DEVICE_COMMAND_IMPL_END() \
    }

#endif // SPI_DEVICE_IMPL_H