
#include <pico/stdlib.h>
#include <stdio.h>
#include <string.h>
#include <w5500.h>
#include <spi.h>

#define MS(x, mask, shift) ((x & mask) << shift)
#define CONCAT16(x1, x2) (x1 << 8 | x2)


#define DEBUG_SPI_TRANSFER 


void w5500_read(spi_device_t *spi, w5500_socket_t s, uint16_t reg, void* data,
                size_t len) {
  uint8_t src[3 + len];
  uint8_t dst[3 + len];

  src[0] = (reg >> 8) & 0xFF;
  src[1] = reg & 0xFF;
  src[2] = MS(s, 0b11111, 3) | MS(0, 0b1, 2) | MS(00, 0b11, 0);
  memset(src + 3, 0, len);

  spi_write_read(spi, src, dst, 3 + len);

#ifdef DEBUG_SPI_TRANSFER
  printf("write: ");
  for (int i = 0; i < 3 + len; i++) {
    printf("%02x ", src[i]);
  }
  printf("\nread: ");
  for (int i = 0; i < 3 + len; i++) {
    printf("%02x ", dst[i]);
  }
  printf("\n");
#endif

  memcpy(data, dst + 3, len);
}


void w5500_write(spi_device_t *spi, w5500_socket_t s, uint16_t reg,
                 const void* data, size_t len) {
  uint32_t src[3 + len];

  src[0] = (reg >> 8) & 0xFF;
  src[1] = reg & 0xFF;
  src[2] = MS(s, 0b11111, 3) | MS(1, 0b1, 2) | MS(00, 0b11, 0);

  memcpy(src + 3, data, len);

  spi_write32(spi, src, 3 + len); 
}


void w5500_reset(spi_device_t *spi) {
  w5500_write8(spi, W5500_COMMON, W5500_MR, 0x80);      // reset
  w5500_write8(spi, W5500_COMMON, W5500_PHYCFGR, 0x0);  // PHY reset
  w5500_write8(spi, W5500_COMMON, W5500_PHYCFGR,
               0xf8);  // PHY set to auto-negotiation
}


bool w5500_ready(spi_device_t *spi) {
  return w5500_read8(spi, W5500_COMMON, W5500_PHYCFGR) & 0x80;  // RST bit
}


bool w5500_has_link(spi_device_t *spi) {
  return w5500_read8(spi, W5500_COMMON, W5500_PHYCFGR) & 1;  // LNK bit
}


void w5500_config(spi_device_t *spi, const mac_t src_mac, const ip_t src_ip,
                  const ip_t subnet_mask, const ip_t gateway) {

  // Physical Layer and Default Options
  uint8_t mode = 0;
  w5500_write8(spi, W5500_COMMON, W5500_MR, mode);

  // // IP and MAC Addresses
  w5500_write(spi, W5500_COMMON, W5500_SIPR0, src_ip, 4);
  w5500_write(spi, W5500_COMMON, W5500_SHAR0, src_mac, 6);
  w5500_write(spi, W5500_COMMON, W5500_SUBR0, subnet_mask, 4);
  w5500_write(spi, W5500_COMMON, W5500_GAR0, gateway, 4);
}


w5500_error_t w5500_create_udp_socket(spi_device_t *spi, w5500_socket_t s,
                                      uint16_t src_port, bool multicast,
                                      bool block_broadcast,
                                      bool block_unicast) {
  w5500_command(spi, s, W5500_CMD_CLOSE);

  // Writing Socket Mode Register
  uint8_t config = (uint8_t)W5500_PROTOCOL_UDP | MS(block_unicast, 1, 4) |
                   MS(block_broadcast, 1, 6) | MS(multicast, 1, 7);
  w5500_write8(spi, s, W5500_Sn_MR, config);

  // Writing Socket Ports
  w5500_write16(spi, s, W5500_Sn_PORT0, src_port);
  w5500_command(spi, s, W5500_CMD_OPEN);

  return SUCCESS;
}


w5500_error_t w5500_create_tcp_socket(spi_device_t *spi, w5500_socket_t s,
                                      uint16_t src_port) {
  w5500_command(spi, s, W5500_CMD_CLOSE);

  // Writing Socket Mode Register
  uint8_t no_delayed_ack = MS(0x01, 0x01, 5);
  uint8_t config         = (uint8_t)W5500_PROTOCOL_TCP | no_delayed_ack;

  // Writing Socket Ports
  w5500_write8(spi, s, W5500_Sn_MR, config);
  w5500_write16(spi, s, W5500_Sn_PORT0, src_port);

  w5500_command(spi, s, W5500_CMD_OPEN);

  return SUCCESS;
}


size_t w5500_read_data(spi_device_t *spi, w5500_socket_t s, uint8_t* data,
                       size_t len) {
  uint16_t avail = w5500_read16(spi, s, W5500_Sn_RX_RSR0);
  if (avail < len) {
    len = avail;
  }
  if (len == 0) {
    return 0;
  }

  uint16_t start_addr = w5500_read16(spi, s, W5500_Sn_RX_RD0);
  w5500_read(spi, s + 2, start_addr, data, len);

  start_addr += len;
  w5500_write16(spi, s, W5500_Sn_RX_RD0, start_addr);

  w5500_command(spi, s, W5500_CMD_RECV);

  return len;
}


w5500_error_t w5500_write_data(spi_device_t *spi, w5500_socket_t s, void* data,
                               size_t len) {
  // get current free size
  uint16_t free_size = w5500_read16(spi, s, W5500_Sn_TX_FSR0);

  // checking if there is enough free space in buffer for the data
  if (free_size <= len) {
    return ERR_INSUFFICIENT_MEM;
  }

  // get current write address
  uint16_t write_addr = w5500_read16(spi, s, W5500_Sn_TX_WR0);

  // write len bytes to tx buffer starting at write_addr
  w5500_write(spi, s + 1, write_addr, data, len);

  // update write address
  write_addr += len;
  w5500_write16(spi, s, W5500_Sn_TX_WR0, write_addr);

  w5500_command(spi, s, W5500_CMD_SEND);

  return SUCCESS;
}


void w5500_command(spi_device_t *spi, w5500_socket_t s,
                   w5500_socket_command_t command) {
  w5500_write8(spi, s, W5500_Sn_CR, command);
}
