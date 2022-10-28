#include <w5500.h>
#include <spi_device_impl.h>
#include <string.h>

#define MS(x, mask, shift) x & mask << shift
#define SET_BITS(x, name) (((x) & name##_MASK) << name##_SHIFT)



#define ADDR_MASK 0x7F //Address Mask
#define ADDR_SHIFT 8

#define BSB_MASK 0x1F //Block Select Bit Mask
#define BSB_SHIFT 3

#define RW_MASK 0x01 //Read/Write Mask
#define RW_SHIFT 3

#define OM_MASK 0x02 //Operation Mode Bits


const uint baudrate = 80 * 1000 * 1000; //80 mHz, max for W5500


SPI_MODE3;
SPI_INITFUNC_IMPL(w5500, baudrate);

//write to a single register
void w5500_wreg(SPI_DEVICE_PARAM, uint8_t reg, block_select_t bsb, void* data, size_t len) 
{
    uint8_t src[len + 2];
    //Setting address and block select bits for transmission
    src[0] = reg;
    src[1] = MS(bsb, BSB_MASK, BSB_SHIFT) & MS(WRITE, RW_MASK, RW_SHIFT) & (0x00 & OM_MASK);
    //Copy data into src and write to register
    memcpy(src+2, data, len);
    SPI_WRITE(src, len);
}

void w5500_rreg(SPI_DEVICE_PARAM, uint8_t reg, block_select_t bsb, void* data, size_t len)
{
    uint8_t dst[len+1];
    uint8_t src[1];
    src[0] = MS(bsb, BSB_MASK, BSB_SHIFT) & MS(WRITE, RW_MASK, RW_SHIFT) & (0x00 & OM_MASK);
    SPI_TRANSFER(src, dst, len);
    memcpy(data, dst, len);
}





//Make buffers pointers
//data buffer is what you want to read to chip or write from chip
//Check datasheet on how phase control works


