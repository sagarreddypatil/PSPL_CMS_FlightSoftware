
#include <pico/stdlib.h>
#include <spi_device_impl.h>
#include <stdio.h>
#include <string.h>
#include <w5500.h>

#define MS(x, mask, shift) ((x & mask) << shift)
#define CONCAT16(x1, x2) (x1 << 8 | x2)

const uint baudrate = 1000 * 1000;  // 80 MHz

SPI_MODE0;
SPI_INITFUNC_IMPL(w5500, baudrate);

void w5500_rw(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg, bool write, void* data, size_t len) {
  uint8_t src[3 + len];
  uint8_t dst[3 + len];

  src[0] = (reg >> 8) & 0xFF;
  src[1] = reg & 0xFF;

  src[2] = MS(s, 0x1F, 3) | MS(write, 0x01, 2) | MS(00, 0x03, 0);

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

void w5500_init(SPI_DEVICE_PARAM, mac_t src_mac, ip_t src_ip, ip_t subnet_mask, ip_t gateway) {
  uint8_t RST = 1 << 7;

  w5500_rw(spi, W5500_COMMON, W5500_MR, true, &RST, 1);   // reset chip
  while (w5500_read8(spi, W5500_COMMON, W5500_MR) & RST)  // wait for reset to complete
    ;

  // // Physical Layer and Default Options
  uint8_t mode = 0;
  w5500_rw(spi, W5500_COMMON, W5500_MR, true, &mode, 1);

  // // IP and MAC Addresses
  w5500_rw(spi, W5500_COMMON, W5500_SIPR0, true, src_ip, 4);
  w5500_rw(spi, W5500_COMMON, W5500_SHAR0, true, src_mac, 6);
  w5500_rw(spi, W5500_COMMON, W5500_SUBR0, true, subnet_mask, 4);
  w5500_rw(spi, W5500_COMMON, W5500_GAR0, true, gateway, 4);
}

w5500_error_t w5500_create_udp_socket(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t src_port, bool multicast, bool block_broadcast, bool block_unicast) {
  w5500_command(spi, s, W5500_CMD_CLOSE);

  // Writing Socket Mode Register
  uint8_t config = (uint8_t)W5500_PROTOCOL_UDP | MS(block_unicast, 1, 4) | MS(block_broadcast, 1, 6) | MS(multicast, 1, 7);
  w5500_write8(spi, s, W5500_Sn_MR, config);

  // Writing Socket Ports
  w5500_write16(spi, s, W5500_Sn_PORT0, src_port);
  w5500_command(spi, s, W5500_CMD_OPEN);

  return SUCCESS;
}

w5500_error_t w5500_create_tcp_socket(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t src_port) {
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

void w5500_wait_socket_status(SPI_DEVICE_PARAM, w5500_socket_t s, w5500_socket_status_t status) {
  while (w5500_read8(spi, s, W5500_Sn_SR) != status)
    ;
}

void w5500_read_data(SPI_DEVICE_PARAM, w5500_socket_t s, uint8_t* data, size_t len) {
}

w5500_error_t w5500_write_data(SPI_DEVICE_PARAM, w5500_socket_t s, void* data, size_t len) {
  // get current free size
  uint16_t free_size = w5500_read16(spi, s, W5500_Sn_TX_FSR0);

  // checking if there is enough free space in buffer for the data
  if (free_size <= len) {
    return ERR_INSUFFICIENT_MEM;
  }

  // get current write address
  uint16_t write_addr = w5500_read16(spi, s, W5500_Sn_TX_WR0);

  // write len bytes to tx buffer starting at write_addr
  w5500_rw(spi, s, write_addr, true, data, len);

  // update write address
  write_addr += len;
  w5500_write16(spi, s, W5500_Sn_TX_WR0, write_addr);

  return SUCCESS;
}

w5500_error_t w5500_command(SPI_DEVICE_PARAM, w5500_socket_t s, w5500_socket_command_t command) {
  // w5500_rw(spi, W5500_Sn_CR, s, &command, 1, true);
  w5500_write8(spi, s, W5500_Sn_CR, command);

#define CHECK_STATUS(x, err) \
  if (status != x) return err;

  // uint8_t status = w5500_read8(spi, s, W5500_Sn_SR);
  // switch (command) {
  //   case W5500_CMD_CLOSE:
  //     CHECK_STATUS(W5500_SOCK_CLOSED, ERR_SOCK_CLOSE)
  //     break;

  //   case W5500_CMD_OPEN:
  //     // uint8_t protocol = w5500_rw(spi, w5500_socket_mr, s, NULL, 1, false) & 0x0F;
  //     uint8_t protocol = w5500_read8(spi, s, W5500_Sn_MR) & 0b1111;

  //     if (protocol == W5500_PROTOCOL_UDP)
  //       CHECK_STATUS(W5500_SOCK_UDP, ERR_INIT_UDP)
  //     else if (protocol == W5500_PROTOCOL_TCP)
  //       CHECK_STATUS(W5500_SOCK_INIT, ERR_INIT_TCP)
  //     else
  //       return ERR_INVALID_PROTOCOL;

  //     break;

  //   case W5500_CMD_LISTEN:
  //     CHECK_STATUS(W5500_SOCK_LISTEN, ERR_INIT_SERVER)
  //     break;

  //   case W5500_CMD_CONNECT:
  //     CHECK_STATUS(W5500_SOCK_ESTABLISHED, ERR_CONNECT_SERVER)
  //     break;
  // }

#undef CHECK_STATUS

  return SUCCESS;
}

/*
For debugging, hopefully will be replaced with some sort of actual debugger at some point
*/

// void w5500_print_reg(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg, uint8_t len) {
//   uint8_t data[6];
//   w5500_rw(spi, reg, s, data, len, false);
//   for (int i = 0; i < len; i++) {
//     printf("0x%x ", data[i]);
//   }
//   printf("\n");
// }

// void w5500_print_all(SPI_DEVICE_PARAM, w5500_socket_t s) {
//   printf("w5500_mr: ");
//   w5500_print_reg(spi, cmn, w5500_mr, 1);
//   printf("w5500_gar: ");
//   w5500_print_reg(spi, cmn, w5500_gar, 4);
//   printf("w5500_subr: ");
//   w5500_print_reg(spi, cmn, w5500_subr, 4);
//   printf("w5500_shar: ");
//   w5500_print_reg(spi, cmn, w5500_shar, 6);
//   printf("w5500_sipr: ");
//   w5500_print_reg(spi, cmn, w5500_sipr, 4);
//   printf("w5500_intLevel: ");
//   w5500_print_reg(spi, cmn, w5500_intLevel, 2);
//   printf("w5500_ir: ");
//   w5500_print_reg(spi, cmn, w5500_ir, 1);
//   printf("w5500_imr: ");
//   w5500_print_reg(spi, cmn, w5500_imr, 1);
//   printf("w5500_sir: ");
//   w5500_print_reg(spi, cmn, w5500_sir, 1);
//   printf("w5500_simr: ");
//   w5500_print_reg(spi, cmn, w5500_simr, 1);
//   printf("w5500_rtr: ");
//   w5500_print_reg(spi, cmn, w5500_rtr, 2);
//   printf("w5500_rcr: ");
//   w5500_print_reg(spi, cmn, w5500_rcr, 1);
//   printf("w5500_ptimer: ");
//   w5500_print_reg(spi, cmn, w5500_ptimer, 1);
//   printf("w5500_pmagic: ");
//   w5500_print_reg(spi, cmn, w5500_pmagic, 1);
//   printf("w5500_phar: ");
//   w5500_print_reg(spi, cmn, w5500_phar, 6);
//   printf("w5500_psid: ");
//   w5500_print_reg(spi, cmn, w5500_psid, 2);
//   printf("w5500_pmru: ");
//   w5500_print_reg(spi, cmn, w5500_pmru, 2);
//   printf("w5500_uipr: ");
//   w5500_print_reg(spi, cmn, w5500_uipr, 4);
//   printf("w5500_uportr: ");
//   w5500_print_reg(spi, cmn, w5500_uportr, 2);
//   printf("w5500_phycfgr: ");
//   w5500_print_reg(spi, cmn, w5500_phycfgr, 1);
//   printf("w5500_versionr: ");
//   w5500_print_reg(spi, cmn, w5500_versionr, 1);

//   printf("w5500_socket_mr: ");
//   w5500_print_reg(spi, s, w5500_socket_mr, 1);
//   printf("w5500_socket_cr: ");
//   w5500_print_reg(spi, s, w5500_socket_cr, 1);
//   printf("w5500_socket_ir: ");
//   w5500_print_reg(spi, s, w5500_socket_ir, 1);
//   printf("w5500_socket_sr: ");
//   w5500_print_reg(spi, s, w5500_socket_sr, 1);
//   printf("w5500_socket_sport: ");
//   w5500_print_reg(spi, s, w5500_socket_sport, 2);
//   printf("w5500_socket_dhar: ");
//   w5500_print_reg(spi, s, w5500_socket_dhar, 6);
//   printf("w5500_socket_dipr: ");
//   w5500_print_reg(spi, s, w5500_socket_dipr, 4);
//   printf("w5500_socket_dport: ");
//   w5500_print_reg(spi, s, w5500_socket_dport, 2);
//   printf("w5500_socket_mssr: ");
//   w5500_print_reg(spi, s, w5500_socket_mssr, 2);
//   printf("w5500_socket_tos: ");
//   w5500_print_reg(spi, s, w5500_socket_tos, 1);
//   printf("w5500_socket_ttl: ");
//   w5500_print_reg(spi, s, w5500_socket_ttl, 1);
//   printf("w5500_socket_rxbuf_size: ");
//   w5500_print_reg(spi, s, w5500_socket_rxbuf_size, 1);
//   printf("w5500_socket_txbuf_size: ");
//   w5500_print_reg(spi, s, w5500_socket_txbuf_size, 1);
//   printf("w5500_socket_tx_fsr: ");
//   w5500_print_reg(spi, s, w5500_socket_tx_fsr, 2);
//   printf("w5500_socket_tx_rd: ");
//   w5500_print_reg(spi, s, w5500_socket_tx_rd, 2);
//   printf("w5500_socket_tx_wr: ");
//   w5500_print_reg(spi, s, w5500_socket_tx_wr, 2);
//   printf("w5500_socket_rsr: ");
//   w5500_print_reg(spi, s, w5500_socket_rsr, 2);
//   printf("w5500_socket_rx_rd: ");
//   w5500_print_reg(spi, s, w5500_socket_rx_rd, 2);
//   printf("w5500_socket_rx_wr: ");
//   w5500_print_reg(spi, s, w5500_socket_rx_wr, 2);
//   printf("w5500_socket_imr: ");
//   w5500_print_reg(spi, s, w5500_socket_imr, 1);
//   printf("w5500_socket_frag: ");
//   w5500_print_reg(spi, s, w5500_socket_frag, 2);
//   printf("w5500_socket_kpalvtr: ");
//   w5500_print_reg(spi, s, w5500_socket_kpalvtr, 1);
// }