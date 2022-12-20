
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

  // printf("%x %x %x %x\n", src[0], src[1], src[2], src[3]);

  SPI_TRANSFER(src, dst, 3 + len);

  if (!write) {
    memcpy(data, dst + 3, len);
  }
}


void w5500_init(SPI_DEVICE_PARAM, ip_t gateway, ip_t sub_mask, ip_t src_ip, mac_t mac_addr) {
  w5500_rw(spi, w5500_gar, cmn, gateway, sizeof(ip_t), true);
  w5500_rw(spi, w5500_subr, cmn, sub_mask, sizeof(ip_t), true);
  w5500_rw(spi, w5500_sipr, cmn, src_ip, sizeof(ip_t), true);
  w5500_rw(spi, w5500_shar, cmn, mac_addr, sizeof(mac_t), true);
}

void w5500_close(SPI_DEVICE_PARAM, w5500_s_t s) {
  uint8_t cr = 0x10;
  w5500_rw(spi, w5500_s_cr, s, &cr, 1, true);
}

void w5500_open(SPI_DEVICE_PARAM, w5500_s_t s) {
  uint8_t cr = 0x01;
  w5500_rw(spi, w5500_s_cr, s, &cr, 1, true);
}
void w5500_status(SPI_DEVICE_PARAM) {
  uint8_t data[6];
  w5500_rw(spi, w5500_gar, cmn, data, sizeof(ip_t), false);
  printf("GATEWAY: %x %x %x %x \n", data[0], data[1], data[2], data[3]);
  w5500_rw(spi, w5500_subr, cmn, data, sizeof(ip_t), false);
  printf("SUBNET MASK: %x %x %x %x\n", data[0], data[1], data[2], data[3]);
  w5500_rw(spi, w5500_sipr, cmn, data, sizeof(ip_t), false);
  printf("IP: %x %x %x %x\n", data[0], data[1], data[2], data[3]);
  w5500_rw(spi, w5500_shar, cmn, data, sizeof(mac_t), false);
  printf("MAC: %x %x %x %x %x %x\n", data[0], data[1], data[2], data[3], data[4], data[5]);
}