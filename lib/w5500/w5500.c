
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

// Phy configuration bits
#define MBPS10 0  // 10 and 100 mbps network speed
#define MBPS100 1

#define AN_D 0  // Auto negotiation disable and enable
#define AN_E 1

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

#define BROADCAST_BLOCK_MASK 0x01  // Broadcast blocking bit
#define BROADCAST_BLOCK_SHIFT 1

#define IGMP_MASK 0x01  // IGMP Version selection bit
#define IGMP_SHIFT 2

#define UNICAST_BLOCK_MASK 0x01  // Unicast blocking bit
#define UNICAST_BLOCK_SHIFT 3

#define PROTOCOL_MASK 0x04  // Protocol selection bits
#define PROTOCOL_SHIFT 4

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

#define PHY_RST_MASK 1
#define PHY_RST_SHIFT 7  // Phy Reset bit

#define PHY_OM_MASK 1
#define PHY_OM_SHIFT 6  // Phy operation mode

#define PHY_AN_MASK 1  // Phy auto negotiation
#define PHY_AN_SHIFT 5

#define PHY_MBPS_MASK 1  // 10baset or 100baset configuration
#define PHY_MBPS_SHIFT 4

#define PHY_DUPLEX_MASK 1
#define PHY_DUPLEX_SHIFT 3

const uint baudrate = 1000 * 1000;  // 1000 kHz

SPI_MODE0;
SPI_INITFUNC_IMPL(w5500, baudrate);

// Read write to register
void w5500_rw(SPI_DEVICE_PARAM, uint16_t reg, w5500_s_t s, void* data, size_t len, bool write) {
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

  //printf("%x %x %x %x\n", src[0], src[1], src[2], src[3]);

  SPI_TRANSFER(src, dst, 3 + len);

  if (!write) {
    memcpy(data, dst + 3, len);
  }
}

void w5500_configIP(SPI_DEVICE_PARAM, ip_addr_t ip, ip_addr_t gateway, ip_addr_t subnet_mask, mac_t mac) {
  // Source IP address configuration
  w5500_rw(spi, w5500_sipr0, cmn, ip, 4, true);
  // Gateway IP address configuration
  w5500_rw(spi, w5500_gar0, cmn, gateway, 4, true);
  // Subnet Mask Configuration
  w5500_rw(spi, w5500_subr0, cmn, subnet_mask, 4, true);
  // Source Hardware Address Configuration
  w5500_rw(spi, w5500_shar0, cmn, mac, 6, true);
}

void w5500_configMR(SPI_DEVICE_PARAM, bool wol, bool ping_block, bool pppoe, bool farp) {
  uint8_t config = MS(0, SW_RESET_MASK, SW_RESET_SHIFT) |
                   MS(wol, WOL_MASK, WOL_SHIFT) |
                   MS(ping_block, PING_BLOCK_MASK, PING_BLOCK_SHIFT) |
                   MS(pppoe, PPPoE_MASK, PPPoE_SHIFT) |
                   MS(farp, FARP_MASK, FARP_SHIFT);
  w5500_rw(spi, w5500_mr, cmn, &config, 1, true);
}
void w5500_s_config(SPI_DEVICE_PARAM, uint16_t src_port, w5500_s_t s, int rxbuf, int txbuf, ip_addr_t dst_ip, uint16_t dst_port) {
  uint8_t src_port_buf[2] = SPLIT16(src_port);
  uint8_t dst_port_buf[2] = SPLIT16(dst_port);
  // Src Port
  w5500_rw(spi, w5500_s_sport0, s, src_port_buf, 2, true);
  // Tx Buffer Size
  w5500_rw(spi, w5500_s_txbuf_size, s, &txbuf, 1, true);
  // Rx Buffer Size
  w5500_rw(spi, w5500_s_rxbuf_size, s, &rxbuf, 1, true);
  //Destination port
  w5500_rw(spi, w5500_s_dport0, s, dst_port_buf, 2, true);
  // Destination IP Address
  w5500_rw(spi, w5500_s_dipr0, s, dst_ip, 4, true);
}

void w5500_s_mode(SPI_DEVICE_PARAM, w5500_s_mr_t protocol, w5500_s_cr_t s_mode,
                   w5500_s_t s, bool multicast, bool unicast_block, bool broadcast_block) {
  uint8_t config = MS(protocol, PROTOCOL_MASK, PROTOCOL_SHIFT) |
                   MS(unicast_block, UNICAST_BLOCK_MASK, UNICAST_BLOCK_SHIFT) |
                   MS(0, IGMP_MASK, IGMP_SHIFT) |  // IGMP Version 2
                   MS(broadcast_block, BROADCAST_BLOCK_MASK, BROADCAST_BLOCK_SHIFT) |
                   (MULTICAST_BLOCK_MASK & multicast);
  w5500_rw(spi, w5500_s_mr, s, &config, 1, true);
  w5500_rw(spi, w5500_s_cr, s, &s_mode, 1, true);
}

uint16_t w5500_s_fs_tx(SPI_DEVICE_PARAM, w5500_s_t s) {  // Reading TX Free Size Range
  uint8_t data[2];
  w5500_rw(spi, w5500_s_tx_fsr0, s, data, 2, false);
  return CONCAT16(data[0], data[1]);
}

void w5500_s_ttl_config(SPI_DEVICE_PARAM, w5500_s_t s, uint8_t ttl) {
  ttl = TTL_MASK & ttl;
  w5500_rw(spi, w5500_s_ttl, s, &ttl, 1, true);
}

void w5500_phy_config(SPI_DEVICE_PARAM, int speed, int duplex, int auto_negotiation) {
  uint8_t config = MS(1, PHY_RST_MASK, PHY_RST_SHIFT) |
                   MS(1, PHY_OM_MASK, PHY_OM_SHIFT) |
                   MS(auto_negotiation, PHY_AN_MASK, PHY_AN_SHIFT) |
                   MS(speed, PHY_MBPS_MASK, PHY_MBPS_SHIFT) |
                   MS(duplex, PHY_DUPLEX_MASK, PHY_DUPLEX_SHIFT);

  w5500_rw(spi, w5500_phycfgr, cmn, &config, 1, true);
}

uint16_t w5500_available(SPI_DEVICE_PARAM, w5500_s_t s) {
  uint8_t data[2];
  // Recieved data size (space used in RX Buffer)
  w5500_rw(spi, w5500_s_rsr0, s, data, 2, false);
  uint16_t data_size = CONCAT16(data[0], data[1]);
  // RX Buffer total size
  w5500_rw(spi, w5500_s_rxbuf_size, s, data, 1, false);
  uint16_t buf_size = data[0] * 1000;
  return (buf_size - data_size);
}

void w5500_s_write(SPI_DEVICE_PARAM, w5500_s_t s, void* data, size_t len) {
  uint8_t start_buf[2];
  // Read starting save address of buffer
  w5500_rw(spi, w5500_s_tx_wr0, s, start_buf, 2, false);
  uint16_t start = CONCAT16(start_buf[0], start_buf[1]);
  // Save data to tx buffer if space available
  if ((start + len) < w5500_s_fs_tx(spi, s)) {
    w5500_rw(spi, start, TXBUF(s), data, len, true);
  }
  // Increment starting save address by data length
  start += len;
  uint8_t newStart[2] = SPLIT16(start);
  memcpy(newStart, start_buf, 2);
  // Save new TX buffer starting address
  w5500_rw(spi, w5500_s_tx_wr0, s, start_buf, 2, true);
}

void w5500_s_transmit(SPI_DEVICE_PARAM, w5500_s_t s) {
  uint8_t data[1] = {w5500_s_send};
  w5500_rw(spi, w5500_s_cr, s, data, 1, true);
}

void w5500_read_rx(SPI_DEVICE_PARAM, w5500_s_t s, void* data) {
  uint8_t addr_buf[3];
  // get starting read address of rx buffer
  w5500_rw(spi, w5500_s_rx_rd0, s, addr_buf, 2, false);
  uint16_t start = CONCAT16(addr_buf[0], addr_buf[1]);

  // get ending read address of rx buffer
  w5500_rw(spi, w5500_s_rx_wr0, s, addr_buf, 2, false);
  uint16_t end = CONCAT16(addr_buf[0], addr_buf[1]);

  // Read all data in rx buffer from start to end address
  for (uint16_t i = start; i < end; i++) {
    w5500_rw(spi, i, RXBUF(s), data, (start - end), true);
  }
  // Update read address of rx buffer
  w5500_rw(spi, w5500_s_rx_rd0, s, addr_buf, 2, true);
  // order recieve command for no reason
  w5500_s_cr_t recv = w5500_s_recv;
  w5500_rw(spi, w5500_s_cr, s, &recv, 1, true);
}
