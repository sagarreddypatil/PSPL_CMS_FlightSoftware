
#include <pico/stdlib.h>
#include <spi_device_impl.h>
#include <stdio.h>
#include <string.h>
#include <w5500.h>

#define MS(x, mask, shift) ((x & mask) << shift)
#define CONCAT16(x1, x2) (x1 << 8 | x2)

#define BSB_TX_OFFSET 1
#define BSB_RX_OFFSET 2

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

#define PROTOCOL_MASK 0x0F  // Protocol selection bits

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

/*
Prototype Macro for error checking in sending data, initializing sockets, etc.
*/
#define ERROR_CHECK(s, reg, expected)               \
  uint8_t err_check_data;                           \
  w5500_rw(spi, reg, s, &err_check_data, 1, false); \
  if (err_check_data != expected) {                 \
    return -1;                                      \
  }

const uint baudrate = 1000 * 1000 * 10;  // 10 MHz

SPI_MODE0;
SPI_INITFUNC_IMPL(w5500, baudrate);

// Read write to register
uint8_t w5500_rw(SPI_DEVICE_PARAM, uint16_t reg, w5500_socket_t s, void* data, size_t len, bool write) {
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
  return 1;
}

uint8_t w5500_init(SPI_DEVICE_PARAM, ip_t gateway, ip_t sub_mask, ip_t src_ip, mac_t mac_addr, bool wol, bool ping_block, bool farp) {
  uint8_t phy = 0xD8;  // 100BT Full Duplex Auto Negotiation Disabled

  uint8_t config =
      MS(wol, WOL_MASK, WOL_SHIFT) |
      MS(ping_block, PING_BLOCK_MASK, PING_BLOCK_SHIFT) |
      MS(0, PPPoE_MASK, PPPoE_SHIFT) |
      MS(farp, FARP_MASK, FARP_SHIFT);

  w5500_rw(spi, w5500_mr, cmn, &config, sizeof(uint8_t), true);
  w5500_rw(spi, w5500_phycfgr, cmn, &phy, sizeof(uint8_t), true);
  w5500_rw(spi, w5500_gar, cmn, gateway, sizeof(ip_t), true);
  w5500_rw(spi, w5500_subr, cmn, sub_mask, sizeof(ip_t), true);
  w5500_rw(spi, w5500_sipr, cmn, src_ip, sizeof(ip_t), true);
  w5500_rw(spi, w5500_shar, cmn, mac_addr, sizeof(mac_t), true);

  return 1;
}

uint8_t w5500_socket_init(SPI_DEVICE_PARAM, w5500_socket_t s, w5500_protocol_t protocol, uint16_t src_port, ip_t dst, uint16_t dst_port, uint8_t txbuf_size, uint8_t rxbuf_size, bool multicast, bool unicast_block, bool broadcast_block, mac_t dhar) {
  uint8_t src_buf[2] = {src_port >> 8, src_port};
  uint8_t dst_buf[2] = {dst_port >> 8, dst_port};
  uint8_t config     = 0x00;

  if (protocol == udp) {
    config = MS(multicast, MULTICAST_BLOCK_MASK, MULTICAST_BLOCK_SHIFT) |
             MS(broadcast_block, BROADCAST_BLOCK_MASK, BROADCAST_BLOCK_SHIFT) |
             MS(0, IGMP_MASK, IGMP_SHIFT) |
             MS(unicast_block, UNICAST_BLOCK_MASK, UNICAST_BLOCK_SHIFT) |
             MS(udp, PROTOCOL_MASK, 0);
  }

  if (protocol == tcp) {
    config = 0x11;
  }

  w5500_rw(spi, w5500_socket_mr, s, &config, sizeof(config), true);

  w5500_cmd_close(spi, s);

  w5500_rw(spi, w5500_socket_sport, s, src_buf, sizeof(src_buf), true);
  w5500_rw(spi, w5500_socket_rxbuf_size, s, &rxbuf_size, 1, true);
  w5500_rw(spi, w5500_socket_txbuf_size, s, &txbuf_size, 1, true);
  w5500_rw(spi, w5500_socket_dipr, s, dst, sizeof(ip_t), true);
  w5500_rw(spi, w5500_socket_dport, s, dst_buf, sizeof(dst_buf), true);

  if (multicast) {
    w5500_rw(spi, w5500_socket_dhar, s, dhar, 6, true);
  }

  w5500_cmd_open(spi, s);

  if (protocol == udp) {
    ERROR_CHECK(s, w5500_socket_sr, w5500_SOCK_UDP);
  }

  if (protocol == tcp) {
    ERROR_CHECK(s, w5500_socket_sr, w5500_SOCK_INIT);
  }
  return 1;
}

uint8_t w5500_write_tx(SPI_DEVICE_PARAM, w5500_socket_t s, void* data, size_t len) {
  uint8_t buf[2];
  uint16_t write_addr;
  uint16_t free_size;

  // get current free size
  w5500_rw(spi, w5500_socket_tx_fsr, s, buf, 2, false);
  free_size = CONCAT16(buf[0], buf[1]);

  // checking if there is enough free space in buffer for the data
  if (free_size <= len) {
    w5500_cmd_send(spi, s);
  }

  // get current write address
  w5500_rw(spi, w5500_socket_tx_wr, s, buf, 2, false);
  write_addr = CONCAT16(buf[0], buf[1]);

  // write len bytes to tx buffer starting at write_addr
  w5500_rw(spi, write_addr, s + BSB_TX_OFFSET, data, len, true);

  // update write address
  write_addr += len;
  buf[0] = write_addr >> 8;
  buf[1] = write_addr;
  w5500_rw(spi, w5500_socket_tx_wr, s, buf, 2, true);
  return 1;
}

uint16_t w5500_recv(SPI_DEVICE_PARAM, w5500_socket_t s, void* recv_buf) {
  uint8_t read_addr_buf[2];
  uint16_t read_addr;
  uint8_t recieved_buf[2];
  uint16_t recieved;

  w5500_rw(spi, w5500_socket_rx_rd, s, read_addr_buf, 2, false);
  read_addr = CONCAT16(read_addr_buf[0], read_addr_buf[1]);

  w5500_rw(spi, w5500_socket_rsr, s, recieved_buf, 2, false);
  recieved = CONCAT16(recieved_buf[0], recieved_buf[1]);

  w5500_rw(spi, read_addr, s + BSB_RX_OFFSET, recv_buf, recieved, false);

  read_addr += recieved;
  read_addr_buf[0] = read_addr >> 8;

  read_addr_buf[1] = read_addr;

  w5500_rw(spi, w5500_socket_rx_rd, s, read_addr_buf, 2, true);

  w5500_cmd_recv(spi, s);

  return recieved;
}

/*
For debugging, hopefully will be replaced with some sort of actual debugger at some point
*/

void w5500_print_reg(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg, uint8_t len) {
  uint8_t data[6];
  w5500_rw(spi, reg, s, data, len, false);
  for (int i = 0; i < len; i++) {
    printf("0x%x ", data[i]);
  }
  printf("\n");
}

void w5500_print_all(SPI_DEVICE_PARAM, w5500_socket_t s) {
  printf("w5500_mr: ");
  w5500_print_reg(spi, cmn, w5500_mr, 1);
  printf("w5500_gar: ");
  w5500_print_reg(spi, cmn, w5500_gar, 4);
  printf("w5500_subr: ");
  w5500_print_reg(spi, cmn, w5500_subr, 4);
  printf("w5500_shar: ");
  w5500_print_reg(spi, cmn, w5500_shar, 6);
  printf("w5500_sipr: ");
  w5500_print_reg(spi, cmn, w5500_sipr, 4);
  printf("w5500_intLevel: ");
  w5500_print_reg(spi, cmn, w5500_intLevel, 2);
  printf("w5500_ir: ");
  w5500_print_reg(spi, cmn, w5500_ir, 1);
  printf("w5500_imr: ");
  w5500_print_reg(spi, cmn, w5500_imr, 1);
  printf("w5500_sir: ");
  w5500_print_reg(spi, cmn, w5500_sir, 1);
  printf("w5500_simr: ");
  w5500_print_reg(spi, cmn, w5500_simr, 1);
  printf("w5500_rtr: ");
  w5500_print_reg(spi, cmn, w5500_rtr, 2);
  printf("w5500_rcr: ");
  w5500_print_reg(spi, cmn, w5500_rcr, 1);
  printf("w5500_ptimer: ");
  w5500_print_reg(spi, cmn, w5500_ptimer, 1);
  printf("w5500_pmagic: ");
  w5500_print_reg(spi, cmn, w5500_pmagic, 1);
  printf("w5500_phar: ");
  w5500_print_reg(spi, cmn, w5500_phar, 6);
  printf("w5500_psid: ");
  w5500_print_reg(spi, cmn, w5500_psid, 2);
  printf("w5500_pmru: ");
  w5500_print_reg(spi, cmn, w5500_pmru, 2);
  printf("w5500_uipr: ");
  w5500_print_reg(spi, cmn, w5500_uipr, 4);
  printf("w5500_uportr: ");
  w5500_print_reg(spi, cmn, w5500_uportr, 2);
  printf("w5500_phycfgr: ");
  w5500_print_reg(spi, cmn, w5500_phycfgr, 1);
  printf("w5500_versionr: ");
  w5500_print_reg(spi, cmn, w5500_versionr, 1);

  printf("w5500_socket_mr: ");
  w5500_print_reg(spi, s, w5500_socket_mr, 1);
  printf("w5500_socket_cr: ");
  w5500_print_reg(spi, s, w5500_socket_cr, 1);
  printf("w5500_socket_ir: ");
  w5500_print_reg(spi, s, w5500_socket_ir, 1);
  printf("w5500_socket_sr: ");
  w5500_print_reg(spi, s, w5500_socket_sr, 1);
  printf("w5500_socket_sport: ");
  w5500_print_reg(spi, s, w5500_socket_sport, 2);
  printf("w5500_socket_dhar: ");
  w5500_print_reg(spi, s, w5500_socket_dhar, 6);
  printf("w5500_socket_dipr: ");
  w5500_print_reg(spi, s, w5500_socket_dipr, 4);
  printf("w5500_socket_dport: ");
  w5500_print_reg(spi, s, w5500_socket_dport, 2);
  printf("w5500_socket_mssr: ");
  w5500_print_reg(spi, s, w5500_socket_mssr, 2);
  printf("w5500_socket_tos: ");
  w5500_print_reg(spi, s, w5500_socket_tos, 1);
  printf("w5500_socket_ttl: ");
  w5500_print_reg(spi, s, w5500_socket_ttl, 1);
  printf("w5500_socket_rxbuf_size: ");
  w5500_print_reg(spi, s, w5500_socket_rxbuf_size, 1);
  printf("w5500_socket_txbuf_size: ");
  w5500_print_reg(spi, s, w5500_socket_txbuf_size, 1);
  printf("w5500_socket_tx_fsr: ");
  w5500_print_reg(spi, s, w5500_socket_tx_fsr, 2);
  printf("w5500_socket_tx_rd: ");
  w5500_print_reg(spi, s, w5500_socket_tx_rd, 2);
  printf("w5500_socket_tx_wr: ");
  w5500_print_reg(spi, s, w5500_socket_tx_wr, 2);
  printf("w5500_socket_rsr: ");
  w5500_print_reg(spi, s, w5500_socket_rsr, 2);
  printf("w5500_socket_rx_rd: ");
  w5500_print_reg(spi, s, w5500_socket_rx_rd, 2);
  printf("w5500_socket_rx_wr: ");
  w5500_print_reg(spi, s, w5500_socket_rx_wr, 2);
  printf("w5500_socket_imr: ");
  w5500_print_reg(spi, s, w5500_socket_imr, 1);
  printf("w5500_socket_frag: ");
  w5500_print_reg(spi, s, w5500_socket_frag, 2);
  printf("w5500_socket_kpalvtr: ");
  w5500_print_reg(spi, s, w5500_socket_kpalvtr, 1);
}