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


//write to registers starting from reg
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
//read from registers starting from reg
void w5500_rreg(SPI_DEVICE_PARAM, uint8_t reg, block_select_t bsb, void* data, size_t len)
{
    uint8_t dst[len+1];
    uint8_t src[1];
    src[0] = MS(bsb, BSB_MASK, BSB_SHIFT) & MS(WRITE, RW_MASK, RW_SHIFT) & (0x00 & OM_MASK);
    SPI_TRANSFER(src, dst, len);
    memcpy(data, dst, len);
}

void w5500_config(SPI_DEVICE_PARAM, ip_addr_t ip, ip_addr_t gateway, ip_addr_t subnet_mask)
{
    //Source IP address configuration
    w5500_wreg(spi, w5500_sipr0, common, ip, 4);
    //Gateway IP address configuration
    w5500_wreg(spi, w5500_gar0, common, gateway, 4);
    //Subnet Mask Configuration
    w5500_wreg(spi, w5500_subr0, common, gateway, 4);
}

void w5500_config_socket( SPI_DEVICE_PARAM, uint8_t src_port[2], block_select_t bsb, 
w5500_socket_mode_t mode, buf_size_t* rxbuf, buf_size_t* txbuf ,
ip_addr_t dst_ip, uint8_t dst_port[2], uint8_t multicast)
{   
    //Src Port
    w5500_wreg(spi, w5500_socket_sport0, bsb, src_port, 2);
    //Destination IP Address
    w5500_wreg(spi, w5500_socket_dipr0, bsb, dst_ip, 4);
    //Destination Port IP Address
    w5500_wreg(spi, w5500_socket_dport0, bsb, dst_port, 2);
    //Tx Buffer Size
    w5500_wreg(spi, w5500_socket_tbuf_size, bsb, txbuf , 1);
    //Rx Buffer Size
    w5500_wreg(spi, w5500_socket_rbuf_size, bsb, rxbuf, 1);
}





//Make buffers pointers
//data buffer is what you want to read to chip or write from chip
//Check datasheet on how phase control works


