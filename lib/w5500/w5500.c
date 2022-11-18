#include <w5500.h>
#include <spi_device_impl.h>
#include <string.h>

#define R 0
#define W 1
#define MS(x, mask, shift) x & mask << shift
#define SET_BITS(x, name) (((x) & name##_MASK) << name##_SHIFT)
#define CONCAT16(x1, x2) (x1 << 8 | x2)
#define TXBUF(sn) (sn + 1)
#define RXBUF(sn) (sn + 2)

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

#define TTL_MASK 0xFF
const uint baudrate = 50 * 1000 * 1000; //50 mHz, max for rp2040

SPI_MODE3;
SPI_INITFUNC_IMPL(w5500, baudrate);

//Read write to register
void w5500_rw(SPI_DEVICE_PARAM, uint8_t reg, w5500_sn_t sn, void* data, size_t len, bool rw)
{
    if(rw)
    {
    uint8_t src[len + 2];
    //Setting address and block select bits for transmission
    src[0] = reg;
    src[1] = MS(sn, SN_MASK, SN_SHIFT) & MS(W, RW_MASK, RW_SHIFT) & (0x00 & OM_MASK);
    //Copy data into src and write to register
    memcpy(src+2, data, len);
    SPI_WRITE(src, len);
    }
    else
    {
    uint8_t dst[len+1];
    uint8_t src[1];
    src[0] = MS(sn, SN_MASK, SN_SHIFT) & MS(R, RW_MASK, RW_SHIFT) & (0x00 & OM_MASK);
    SPI_TRANSFER(src, dst, len);
    memcpy(data, dst, len);
    }
}

void w5500_config(SPI_DEVICE_PARAM, ip_addr_t ip, ip_addr_t gateway, ip_addr_t subnet_mask, mac_t mac)
{
    //Source IP address configuration
    w5500_rw(spi, w5500_sipr0, cmn, ip, 4, W);
    //Gateway IP address configuration
    w5500_rw(spi, w5500_gar0, cmn, gateway, 4, W);
    //Subnet Mask Configuration
    w5500_rw(spi, w5500_subr0, cmn, gateway, 4, W);
    //Source Hardware Address Configuration
    w5500_rw(spi, w5500_shar0, cmn, mac, 6, W);
}

void w5500_sn_config(SPI_DEVICE_PARAM, uint16_t src_port, w5500_sn_t sn, int rxbuf, int txbuf , ip_addr_t dst_ip, uint16_t dst_port)
{   
    uint8_t src_port_buf[3] = {src_port >> 8, src_port};
    
    //Src Port
    w5500_rw(spi, w5500_sn_sport0, sn, src_port_buf, 2, W);
    //Tx Buffer Size
    w5500_rw(spi, w5500_sn_txbuf_size, sn, &txbuf , 1, W);
    //Rx Buffer Size
    w5500_rw(spi, w5500_sn_rxbuf_size, sn, &rxbuf, 1, W);
}

void w5500_sn_dst(SPI_DEVICE_PARAM, w5500_sn_t sn, ip_addr_t dst_ip, uint16_t dst_port)
{
    uint8_t dst_port_buf[3] = {dst_port >> 8, dst_port};
    w5500_rw(spi, w5500_sn_dport0, sn, dst_port_buf, 2, W);
    //Destination IP Address
    w5500_rw(spi, w5500_sn_dipr0, sn, dst_ip, 4, W);
}

void w5500_sn_mode(SPI_DEVICE_PARAM, w5500_sn_mr_t protocol, w5500_sn_cr_t sn_mode, 
w5500_sn_t sn, bool multicast, bool unicast_block, bool broadcast_block)
{
    uint8_t config = MS(protocol, PROTOCOL_MASK, PROTOCOL_SHIFT) & 
                     MS(unicast_block, UNICAST_BLOCK_MASK, UNICAST_BLOCK_SHIFT) &
                     MS(0, IGMP_MASK, IGMP_SHIFT) & //IGMP Version 2
                     MS(broadcast_block, BROADCAST_BLOCK_MASK, BROADCAST_BLOCK_SHIFT) &
                     (MULTICAST_BLOCK_MASK & multicast);
    w5500_rw(spi, w5500_sn_mr, sn, &config, 1, W);
    w5500_rw(spi, w5500_sn_cr, sn, &sn_mode, 1, W);
}

uint16_t w5500_sn_fs_tx(SPI_DEVICE_PARAM, w5500_sn_t sn)
{   //Reading TX Free Size Range
    uint8_t data[3];
    w5500_rw(spi, w5500_sn_tx_fsr0, sn, data, 2, R);
    return CONCAT16(data[0], data[1]);
}

void w5500_sn_ttl_config(SPI_DEVICE_PARAM, w5500_sn_t sn, uint8_t ttl)
{
    ttl = TTL_MASK & ttl;
    w5500_rw(spi, w5500_sn_ttl, sn, &ttl, 1, W);
}

uint16_t w5500_available(SPI_DEVICE_PARAM, w5500_sn_t sn)
{   
    uint8_t data[3];
    //Recieved data size (space used in RX Buffer)
    w5500_rw(spi, w5500_sn_rsr0, sn, data, 2, R);
    uint16_t data_size = CONCAT16(data[0], data[1]);
    //RX Buffer total size
    w5500_rw(spi, w5500_sn_rxbuf_size, sn, data, 1, R);
    uint16_t buf_size = data[0] * 1000;
    return (buf_size - data_size);
}

void w5500_sn_write(SPI_DEVICE_PARAM, w5500_sn_t sn, void* data, size_t len)
{  
    uint8_t start_buf[3]; 
    //Read starting save address of buffer
    w5500_rw(spi, w5500_sn_tx_wr0, sn, start_buf, 2, R);
    uint16_t start = CONCAT16(start_buf[0], start_buf[1]);
    //Save data to tx buffer if space available
    if((start + len) < w5500_sn_fs_tx(spi, sn))
    {
        w5500_rw(spi, start, TXBUF(sn), data, len, W);
    }
    //Increment starting save address by data length
    start+=len;
    start_buf[0] = start << 8;
    start_buf[1] = start;
    //Save new TX buffer starting address
    w5500_rw(spi, w5500_sn_tx_wr0, sn, start_buf, 2, W);
}

void w5500_sn_transmit(SPI_DEVICE_PARAM, w5500_sn_t sn)
{
    uint8_t data[1] = {w5500_sn_send};
    w5500_rw(spi, w5500_sn_cr, sn, data, 1, W);
}

void w5500_read_rx(SPI_DEVICE_PARAM, w5500_sn_t sn, void* data)
{
    uint8_t addr_buf[3]; 
    //get starting read address of rx buffer
    w5500_rw(spi, w5500_sn_rx_rd0, sn, addr_buf, 2, R);
    uint16_t start = CONCAT16(addr_buf[0], addr_buf[1]);

    //get ending read address of rx buffer
    w5500_rw(spi, w5500_sn_rx_wr0, sn, addr_buf, 2, R);
    uint16_t end = CONCAT16(addr_buf[0], addr_buf[1]);

    //Read all data in rx buffer from start to end address
    for(uint16_t i = start; i < end; i++)
    {
        w5500_rw(spi, i, RXBUF(sn), data, (start - end), W);
    }
    //Update read address of rx buffer
    w5500_rw(spi, w5500_sn_rx_rd0, sn, addr_buf, 2, W);
    //order recieve command for no reason
    w5500_sn_cr_t recv  = w5500_sn_recv;
    w5500_rw(spi, w5500_sn_cr, sn, &recv, 1, W);
}

