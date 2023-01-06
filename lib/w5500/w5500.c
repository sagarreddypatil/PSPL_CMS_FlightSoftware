
#include <pico/stdlib.h>
#include <spi_device_impl.h>
#include <stdio.h>
#include <string.h>
#include <w5500.h>

#define MS(x, mask, shift) ((x & mask) << shift)
#define CONCAT16(x1, x2) (x1 << 8 | x2)
#define SPLIT16(x1) \
  { x1 >> 8, x1 }

#define TXBUF(s) (s + 1)
#define RXBUF(s) (s + 2)



#define HALF_DUPLEX 0  // Half and Full duplex
#define FULL_DUPLEX 1

// Bitmasks & shifts
#define ADDR_MASK 0x7F  // Address Mask
#define ADDR_SHIFT 7

#define BSB_MASK 0x1F  // Block Select Bit Mask
#define BSB_SHIFT 3

#define RW_MASK 0x01  // Read/Write Mask
#define RW_SHIFT 2

#define OM_MASK 0x02  // Operation Mode Bits
#define OM_SHIFT 0

#define MULTICAST_BLOCK_MASK 0x01  // Multicast Bit
#define MULTICAST_BLOCK_SHIFT 7

#define BROADCAST_BLOCK_MASK 0x01  // Broadcast blocking bit
#define BROADCAST_BLOCK_SHIFT 6

#define IGMP_MASK 0x01  // IGMP Version selection bit
#define IGMP_SHIFT 2

#define UNICAST_BLOCK_MASK 0x01  // Unicast blocking bit
#define UNICAST_BLOCK_SHIFT 4

#define PROTOCOL_MASK 0x04  // Protocol selection bits

#define SW_RESET_MASK 0x01  // Software Reset Bit (resets chip to default)
#define SW_RESET_SHIFT 7

#define WOL_MASK 0x01  // Wake on LAN bit
#define WOL_SHIFT 5

#define PING_BLOCK_MASK 0x01  // Ping Block Bit
#define PING_BLOCK_SHIFT 4

#define PPPoE_MASK 0x01  // PPPoE enable/disable bit
#define PPPoE_SHIFT 3

#define FARP_MASK 0x01  // Force ARP Request
#define FARP_SHIFT 1

#define TTL_MASK 0xFF  // Time to live of IP header


const uint baudrate = 1000 * 1000;  // 1000 kHz

SPI_MODE0;
SPI_INITFUNC_IMPL(w5500, baudrate);

// Read write to register
void w5500_rw(SPI_DEVICE_PARAM, uint16_t reg, w5500_socket_t s, void* data, size_t len, bool write) {
  uint8_t src[3 + len];
  uint8_t dst[3 + len];

  src[0] = (reg >> 8) & 0xFF;
  src[1] = reg & 0xFF;

  src[2] = MS(s, BSB_MASK, BSB_SHIFT) | MS(write, RW_MASK, RW_SHIFT) | MS(00, OM_MASK, OM_SHIFT);

  if (write) {
    memcpy(src + 3, data, len);
  } else {
    memset(src + 3, 0, len);
  }

  SPI_TRANSFER(src, dst, 3 + len);

  if (!write) {
    memcpy(data, dst + 3, len);
  }
}

void w5500_read_tx(SPI_DEVICE_PARAM, w5500_socket_t s, size_t len, void* data) {
  uint8_t read_addr[2];
  w5500_rw(spi, w5500_socket_tx_rd, s, read_addr, 2, false);
  w5500_rw(spi, 0, TXBUF(s), data, len, false);
}

void w5500_write_tx(SPI_DEVICE_PARAM, w5500_socket_t s, void* data, size_t len) {
  uint8_t write_addr_buf[2];
  uint16_t write_addr;
  w5500_rw(spi, w5500_socket_tx_wr, s, write_addr_buf, 2, false);
  write_addr = CONCAT16(write_addr_buf[0], write_addr_buf[1]);
  w5500_rw(spi, write_addr, TXBUF(s), data, len, true);
  printf("WRITE ADDR %x\n", write_addr);
  write_addr+=len;
  write_addr_buf[0] = write_addr >> 8;
  write_addr_buf[1] = write_addr;
  w5500_rw(spi, w5500_socket_tx_wr, s, write_addr_buf, 2, true);
}

void w5500_recv(SPI_DEVICE_PARAM, w5500_socket_t s) {
  
}

void w5500_send(SPI_DEVICE_PARAM, w5500_socket_t s) {
  uint8_t send = 0x20;
  w5500_rw(spi, w5500_socket_cr, s, &send, 1, true);
  
}

void w5500_init(SPI_DEVICE_PARAM, ip_t gateway, ip_t sub_mask, ip_t src_ip, mac_t mac_addr) {
  uint8_t mr = 0x02; //Default w5500 options
  uint8_t phy = 0xD8; //100BT Full Duplex Auto Negotiation Disabled
  w5500_rw(spi, w5500_phycfgr, cmn, &phy, sizeof(uint8_t), true);
  w5500_rw(spi, w5500_mr, cmn, &mr, sizeof(uint8_t), true);
  w5500_rw(spi, w5500_gar, cmn, gateway, sizeof(ip_t), true);
  w5500_rw(spi, w5500_subr, cmn, sub_mask, sizeof(ip_t), true);
  w5500_rw(spi, w5500_sipr, cmn, src_ip, sizeof(ip_t), true);
  w5500_rw(spi, w5500_shar, cmn, mac_addr, sizeof(mac_t), true);
  
}

void w5500_socket_init(SPI_DEVICE_PARAM, w5500_socket_t s, ip_t dst_ip, uint16_t src_port, uint16_t dst_port, uint8_t txbuf_size, uint8_t rxbuf_size) {
  uint8_t src_buf[2] = SPLIT16(src_port);
  uint8_t dst_buf[2] = SPLIT16(dst_port);
  uint8_t mr = 0x02; //Default UDP socket options
  //uint8_t mr = 0x81; // Default tcp 
  w5500_rw(spi, w5500_socket_mr, s, &mr, sizeof(mr), true);
  w5500_close(spi, s);
  w5500_rw(spi, w5500_socket_sport, s, src_buf, sizeof(src_buf), true);
  w5500_rw(spi, w5500_socket_dport, s, dst_buf, sizeof(dst_buf), true);
  w5500_rw(spi, w5500_socket_dipr, s, dst_ip, sizeof(ip_t), true);
  w5500_rw(spi, w5500_socket_rxbuf_size, s, &rxbuf_size, 1, true);
  w5500_rw(spi, w5500_socket_txbuf_size, s, &txbuf_size, 1, true);
  w5500_open(spi, s);
}

void w5500_config(SPI_DEVICE_PARAM, bool wol, bool ping_block, bool pppoe, bool farp) {
  uint8_t config = MS(wol, WOL_MASK, WOL_SHIFT) | 
                   MS(ping_block, PING_BLOCK_MASK, PING_BLOCK_SHIFT) |
                   MS(pppoe, PPPoE_MASK, PPPoE_SHIFT) |
                   MS(farp, FARP_MASK, FARP_SHIFT);
  w5500_rw(spi, w5500_mr, cmn, &config, sizeof(uint8_t), true);
}

void w5500_socket_config(SPI_DEVICE_PARAM, w5500_socket_t sn, w5500_protocol_t protocol, bool multicast, bool unicast_block, bool broadcast_block) {
  uint8_t config = MS(multicast, MULTICAST_BLOCK_MASK, MULTICAST_BLOCK_SHIFT) |
                   MS(broadcast_block, BROADCAST_BLOCK_MASK, BROADCAST_BLOCK_SHIFT) |
                   MS(unicast_block, UNICAST_BLOCK_MASK, UNICAST_BLOCK_SHIFT) |
                   MS(protocol, PROTOCOL_MASK, 0);
  w5500_rw(spi, w5500_socket_mr, cmn, &config, sizeof(uint8_t), true);
}

void w5500_set_dst(SPI_DEVICE_PARAM, w5500_socket_t sn, ip_t dst) {
  w5500_close(spi, sn);
  w5500_rw(spi, w5500_socket_dipr, sn, dst, sizeof(ip_t), true);
  w5500_open(spi, sn);
}

void w5500_close(SPI_DEVICE_PARAM, w5500_socket_t s) {
  uint8_t cr = 0x10;
  w5500_rw(spi, w5500_socket_cr, s, &cr, 1, true);
}

void w5500_open(SPI_DEVICE_PARAM, w5500_socket_t s) {
  uint8_t cr = 0x01;
  w5500_rw(spi, w5500_socket_cr, s, &cr, 1, true);
}

void w5500_status(SPI_DEVICE_PARAM, w5500_socket_t sn) {
  uint8_t data[6];
  if(sn == cmn) 
  {
    w5500_rw(spi, w5500_gar, cmn, data, sizeof(ip_t), false);
    printf("GATEWAY: %x %x %x %x \n", data[0], data[1], data[2], data[3]);

    w5500_rw(spi, w5500_subr, cmn, data, sizeof(ip_t), false);
    printf("SUBNET MASK: %x %x %x %x\n", data[0], data[1], data[2], data[3]);

    w5500_rw(spi, w5500_sipr, cmn, data, sizeof(ip_t), false);
    printf("IP: %x %x %x %x\n", data[0], data[1], data[2], data[3]);

    w5500_rw(spi, w5500_shar, cmn, data, sizeof(mac_t), false);
    printf("MAC: %x %x %x %x %x %x\n", data[0], data[1], data[2], data[3], data[4], data[5]);
  }

  else 
  {
    w5500_rw(spi, w5500_socket_dipr, sn, data, sizeof(ip_t), false);
    printf("DST IP: %x %x %x %x\n", data[0], data[1], data[2], data[3]);
    w5500_rw(spi, w5500_socket_sport, sn, data, sizeof(uint16_t), false);
    printf("SRC PORT: %x %x \n", data[0], data[1]);
    w5500_rw(spi, w5500_socket_dport, sn, data, sizeof(uint16_t), false);
    printf("DST PORT: %x %x \n", data[0], data[1]);

    
  }


}

void w5500_print_reg(SPI_DEVICE_PARAM, w5500_socket_t sn, uint16_t reg, uint8_t len) {
  uint8_t data[6];
  w5500_rw(spi, reg, sn, data, len, false);
  for(int i = 0; i < len; i++) {
    printf("0x%x ", data[i]);
  }
  printf("\n");
}