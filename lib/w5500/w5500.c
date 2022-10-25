#include <w5500.h>
#include <spi_device_impl.h>

const uint baudrate = 80 * 1000 * 1000; //80 mHz, max for W5500




SPI_MODE3;
SPI_INITFUNC_IMPL(w5500, baudrate);

//read a single register from w5500
uint8_t w5500_rreg_byte(SPI_DEVICE_PARAM, uint8_t reg) {

}



