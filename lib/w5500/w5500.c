#include <w5500.h>
#include <spi_device_impl.h>

#define MS(x, mask, shift) x & mask << shift
#define SET_BITS(x, name) (((x) & name##_MASK) << name##_SHIFT)

#define ADDR_MASK 0x7F //Address Mask
#define ADDR_SHIFT 8

#define BSB_MASK 0x1F //Block Select Bit Mask
#define BSB_SHIFT 3

#define READ_MASK 0x01 //Read/Write Mask
#define READ_SHIFT 3

#define OM_MASK 0x02 //Operation Mode Bits


const uint baudrate = 80 * 1000 * 1000; //80 mHz, max for W5500

SPI_MODE3;
SPI_INITFUNC_IMPL(w5500, baudrate);

//read & write data to any register on the w5500
void w5500_transfer(SPI_DEVICE_PARAM, uint8_t reg, block_select_t bsb, uint8_t rw, void* data, size_t len) 
{
    uint8_t src[len + 1]; //rp2040 DO to w5500 DI
    uint8_t dst[len + 1]; //chip DO to rp2040 DI

    //Address Phase of Transmission
    src[0] = reg;
    SPI_TRANSFER(src, dst, 1);
    //Control Phase of Transmission
    src[0] = MS(bsb, BSB_MASK, BSB_SHIFT) & MS(rw, READ_MASK, READ_SHIFT) & (0x00 & OM_MASK);
    SPI_TRANSFER(src, dst, 1);
    //Data Phase of Transmission
    SPI_TRANSFER(data, dst, len);
}



