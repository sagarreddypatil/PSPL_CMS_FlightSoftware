#include <w5500.h>
#include <spi_device_impl.h>
#include <string.h>

#define MS(x, mask, shift) x & mask << shift
#define SET_BITS(x, name) (((x) & name##_MASK) << name##_SHIFT)

#define ADDR_MASK 0x7F //Address Mask
#define ADDR_SHIFT 7

#define SN_MASK 0x1F //Block Select Bit Mask
#define SN_SHIFT 3

#define RW_MASK 0x01 //Read/Write Mask
#define RW_SHIFT 3

#define OM_MASK 0x02 //Operation Mode Bits

#define MULTICAST_BLOCK_MASK 0x01 //Multicast Bit

#define BROADCAST_BLOCK_MASK 0x01 //Broadcast blocking bit
#define BROADCAST_BLOCK_SHIFT 1

#define IGMP_MASK 0x01 //IGMP Version selection bit
#define IGMP_SHIFT 2

#define UNICAST_BLOCK_MASK 0x01 //Unicast blocking bit
#define UNICAST_BLOCK_SHIFT 3

#define PROTOCOL_MASK 0x04 //Protocol selection bits
#define PROTOCOL_SHIFT 4

#define SW_RESET_MASK 0x01 //Software Reset Bit (resets chip to default)
#define SW_RESET_SHIFT 7

#define WOL_MASK 0x01 //Wake on LAN bit
#define WOL_SHIFT 5 

#define PING_BLOCK_MASK 0x01 //Ping Block Bit
#define PING_BLOCK_SHIFT 4

#define PPPoE_MASK 0x01 //PPPoE enable/disable bit
#define PPPoE_SHIFT 3

#define FARP_MASK 0x01 //Force ARP Request
#define FARP_SHIFT 1
const uint baudrate = 50 * 1000 * 1000; //50 mHz, max for rp2040

SPI_MODE3;
SPI_INITFUNC_IMPL(w5500, baudrate);


//write to registers starting from reg
void w5500_wreg(SPI_DEVICE_PARAM, uint8_t reg, w5500_sn_t sn, void* data, size_t len)
{
    uint8_t src[len + 2];
    //Setting address and block select bits for transmission
    src[0] = reg;
    src[1] = MS(sn, SN_MASK, SN_SHIFT) & MS(WRITE, RW_MASK, RW_SHIFT) & (0x00 & OM_MASK);
    //Copy data into src and write to register
    memcpy(src+2, data, len);
    SPI_WRITE(src, len);
}
//read from registers starting from reg
void w5500_rreg(SPI_DEVICE_PARAM, uint8_t reg, w5500_sn_t sn, void* data, size_t len)
{
    uint8_t dst[len+1];
    uint8_t src[1];
    src[0] = MS(sn, SN_MASK, SN_SHIFT) & MS(WRITE, RW_MASK, RW_SHIFT) & (0x00 & OM_MASK);
    SPI_TRANSFER(src, dst, len);
    memcpy(data, dst, len);
}

void w5500_config(SPI_DEVICE_PARAM, ip_addr_t ip, ip_addr_t gateway, ip_addr_t subnet_mask,  bool wol)
{
    //Source IP address configuration
    w5500_wreg(spi, w5500_sipr0, common, ip, 4);
    //Gateway IP address configuration
    w5500_wreg(spi, w5500_gar0, common, gateway, 4);
    //Subnet Mask Configuration
    w5500_wreg(spi, w5500_subr0, common, gateway, 4);
    //Mode Register Configuration
    uint8_t config = MS(1, SW_RESET_MASK, SW_RESET_SHIFT) &
                     MS(wol, WOL_MASK, WOL_SHIFT) &
                     MS(1, PING_BLOCK_MASK, PING_BLOCK_SHIFT) & //Not sure if we should block ping requests
                     MS(0, PPPoE_MASK, PPPoE_SHIFT) &
                     MS(0, FARP_MASK, FARP_SHIFT);
    w5500_wreg(spi, w5500_mr, common, &config);
}

void w5500_config_socket( SPI_DEVICE_PARAM, uint16_t src_port, w5500_sn_t sn, 
w5500_sn_buf_size_t rxbuf, w5500_sn_buf_size_t txbuf , ip_addr_t dst_ip, uint16_t dst_port)
{   
    uint8_t src_port_buf[3] = {src_port >> 8, src_port};
    uint8_t dst_port_buf[3] = {dst_port >> 8, dst_port};
    
    //Src Port
    w5500_wreg(spi, w5500_socket_sport0, sn, src_port_buf, 2);
    //Destination Port IP Address
    w5500_wreg(spi, w5500_socket_dport0, sn, dst_port_buf, 2);
    //Destination IP Address
    w5500_wreg(spi, w5500_socket_dipr0, sn, dst_ip, 4);
    //Tx Buffer Size
    w5500_wreg(spi, w5500_socket_txbuf_size, sn, &txbuf , 1);
    //Rx Buffer Size
    w5500_wreg(spi, w5500_socket_rxbuf_size, sn, &rxbuf, 1);
}

void w5500_socket_mode(SPI_DEVICE_PARAM, w5500_sn_mr_t protocol, w5500_sn_cr_t cr, 
w5500_sn_t sn, bool multicast, bool broadcast_block, bool unicast_block)
{   //Creating Mode Byte
    uint8_t config = MS(protocol, PROTOCOL_MASK, PROTOCOL_SHIFT) & 
                     MS(unicast_block, UNICAST_BLOCK_MASK, UNICAST_BLOCK_SHIFT) &
                     MS(0, IGMP_MASK, IGMP_SHIFT) & //IGMP Version 2
                     MS(broadcast_block, BROADCAST_BLOCK_MASK, BROADCAST_BLOCK_SHIFT) &
                     (MULTICAST_BLOCK_MASK & multicast);
    //Writing to Mode Register
    w5500_wreg(spi, w5500_socket_mr, sn, &config, 1);
    //Writing to Command Register
    w5500_wreg(spi, w5500_socket_cr, sn, &cr, 1);
}

uint16_t w5500_free_tx(SPI_DEVICE_PARAM, w5500_sn_t sn)
{   //Reading TX Free Size Range
    uint8_t data[2];
    w5500_rreg(spi, w5500_socket_tx_fsr0, sn, data, 2);
    return (data[0] << 8 & data[1]);
}




