
#include <pico/stdlib.h>
#include <spi_device_impl.h>
#include <stdio.h>
#include <string.h>
#include <w5500.h>

#define MS(x, mask, shift) ((x & mask) << shift)
#define CONCAT16(x1, x2) (x1 << 8 | x2)

#define BSB_TX_OFFSET 1 


const uint baudrate = 1000;  // 80 MHz

SPI_MODE0;
SPI_INITFUNC_IMPL(w5500, baudrate);


uint8_t w5500_rw(SPI_DEVICE_PARAM, uint16_t reg, w5500_socket_t s, void* data, size_t len, bool write) {
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
  return 1;
}


uint8_t w5500_create(SPI_DEVICE_PARAM, ip_t src_ip, mac_t src_mac, ip_t subnet_mask, ip_t gateway, uint8_t retry_time, uint8_t retry_count, bool wake_on_LAN, bool block_ping, bool force_ARP) {
  
  uint8_t rst = MS(0x00, 0x01, 7);
  uint8_t ms_wake_on_LAN = MS(wake_on_LAN, 0x01, 5);
  uint8_t ms_block_ping = MS(block_ping, 0x01, 4);
  uint8_t ms_pppoe = MS(0x00, 0x01, 3);
  uint8_t ms_force_ARP = MS(force_ARP, 0x01, 1);

  uint8_t config = rst | ms_wake_on_LAN | ms_block_ping | ms_pppoe | ms_force_ARP;

  //Physical Layer and Default Options
  w5500_rw(spi, w5500_mr, cmn, &config, 1, 1);

  //IP and MAC Addresses
  w5500_rw(spi, w5500_sipr, cmn, src_ip, 4, 1);
  w5500_rw(spi, w5500_shar, cmn, src_mac, 6, 1);
  w5500_rw(spi, w5500_subr, cmn, subnet_mask, 4, 1);
  w5500_rw(spi, w5500_gar, cmn, gateway, 4, 1);

  retry_time =  retry_time * 10;
  //Retry Time and Count for failed transmissions
  w5500_rw(spi, w5500_rtr, cmn, &retry_time,  1, 1);
  w5500_rw(spi, w5500_rcr, cmn, &retry_count, 1, 1);

  return 1;

}

uint8_t w5500_create_socket(SPI_DEVICE_PARAM, w5500_socket_t s, w5500_protocol_t protocol, bool multicast, ip_t dst_addr,  uint16_t src_port, uint16_t dst_port, uint8_t rx_buffer_size, uint8_t tx_buffer_size, bool block_broadcast, bool block_unicast) {

  if(protocol != udp && multicast) return 0;

  w5500_command(spi, s, CMD_CLOSE);

  uint8_t ms_block_unicast = MS(block_unicast, 0x01, 4);
  uint8_t ms_no_delayed_ack = MS(0x00, 0x01, 5);
  uint8_t ms_block_broadcast = MS(block_broadcast, 0x01, 6);
  uint8_t ms_multicast = MS(multicast, 0x01, 7);

  //Writing Socket Mode Register
  uint8_t config = ms_multicast | ms_block_broadcast | ms_no_delayed_ack | ms_block_unicast | protocol;

  w5500_rw(spi, w5500_socket_mr, s, &config, 1, 1);
  
  //Writing Socket Ports
  uint8_t port_buf[2];

  w5500_split16(dst_port, port_buf);

  w5500_rw(spi, w5500_socket_dport, s, port_buf, 2, 1);

  w5500_split16(src_port, port_buf);

  w5500_rw(spi, w5500_socket_sport, s, port_buf, 2, 1);
  
  //Writing Socket destination ip
  w5500_rw(spi, w5500_socket_dipr, s, dst_addr, 4, 1);

  uint8_t temp = w5500_command(spi, s, CMD_OPEN);

  if(temp != -1) {
    printf("%x\n", temp);
    return 0;
  }

  return 1;

}

uint8_t w5500_write_data(SPI_DEVICE_PARAM, w5500_socket_t s, void* data, size_t length) {

  uint8_t buf[2];
  uint16_t write_addr;
  uint16_t free_size;

  // get current free size
  w5500_rw(spi, w5500_socket_tx_fsr, s, buf, 2, false);
  free_size = w5500_concat16(buf);

  // checking if there is enough free space in buffer for the data
  if (free_size <= length) {
    return 0;
  }

  // get current write address
  w5500_rw(spi, w5500_socket_tx_wr, s, buf, 2, false);
  write_addr = w5500_concat16(buf);

  // write len bytes to tx buffer starting at write_addr
  w5500_rw(spi, write_addr, s + BSB_TX_OFFSET, data, length, true);

  // update write address
  write_addr += length;
  w5500_split16(write_addr, buf);

  w5500_rw(spi, w5500_socket_tx_wr, s, buf, 2, true);
  return 1;
}

uint8_t w5500_command(SPI_DEVICE_PARAM, w5500_socket_t s, uint8_t command) {

  w5500_rw(spi, w5500_socket_cr, s, &command, 1, 1);

  while(w5500_check_status(spi, s) != SOCK_CLOSED) {
    uint8_t x = w5500_check_interrupt(spi, s);
    if(x >= INT_TIMEOUT) {
      return x;
    }
  }
  return -1;
}


uint8_t w5500_split16(uint16_t num, uint8_t* dst) {
  dst[0] = MS(num, 0xFF, 0);
  dst[1] = MS(num, 0xFF, 1);
  return 1;
}

uint16_t w5500_concat16(uint8_t* num) {
  return (num[0] << 8 | num[1]);
}


uint8_t w5500_check_status(SPI_DEVICE_PARAM, w5500_socket_t s) {
  uint8_t status = -1;
  w5500_rw(spi, w5500_socket_sr, s, &status, 1, 0);
  return status;
}

uint8_t w5500_check_interrupt(SPI_DEVICE_PARAM, w5500_socket_t s) {
  uint8_t interrupt = -1;
  w5500_rw(spi, w5500_socket_ir, s, &interrupt, 1, 0);
  return interrupt;
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