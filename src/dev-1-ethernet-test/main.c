#include <pico/stdlib.h>
#include <stdio.h>
#include <w5500.h>

SPI_DEVICE(w5500, spi0, 0);
int main() {
  stdio_init_all();

  spi_init(spi0, 1000 * 1000);
  gpio_set_function(2, GPIO_FUNC_SPI);
  gpio_set_function(3, GPIO_FUNC_SPI);
  gpio_set_function(4, GPIO_FUNC_SPI);

  for (int i = 0; i < 10; i++) {
    printf("debug point 0 %d\n", i);
    sleep_ms(1000);
  }

  uint actual_baud = w5500_set(w5500);
  ip_t gateway = {192,168,2,50};
  ip_t subnet = {0xff, 0xff, 0xff, 0x00};
  ip_t src = {192, 168, 2, 100};
  mac_t mac = {0x08, 0xDC, 0x0A, 0x01, 0x10, 0x18};
  ip_t s0_dst = {192, 168, 2, 50};
  uint16_t s0_src_port = 5000;
  uint16_t s0_dst_port = 5353;
  uint8_t s0_data[4] = {0x02,0x05,0x08,0x09};
  //uint8_t ir[4] = {0};
  //uint8_t s0_read_data[4] = {0};
  //mac_t dhar = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  printf("debug point 1, actual baud %d\n", actual_baud);
  w5500_init(w5500, gateway, subnet, src, mac);
  w5500_socket_init(w5500, s1, s0_dst, s0_src_port, s0_dst_port, 8, 8);

  w5500_write_tx(w5500, s1, s0_data, 4);
  w5500_send(w5500, s1);
  // while (true) {
  
  // w5500_send(w5500,s1); 
  // w5500_rw(w5500, w5500_ir, s1, ir, 1, false);
  // w5500_rw(w5500, w5500_socket_ir, s1,ir+1, 1, false);
  // printf("%x\n", ir[0]);
  // printf("%x\n", ir[1]);
  // sleep_ms(500);
    
    
    // w5500_rw(w5500 , w5500_mr, cmn, &data, 1, false);
    // printf("0x%x\n", data);
  printf("w5500_mr: ");
  w5500_print_reg(w5500, cmn, w5500_mr, 1);
  printf("w5500_gar: ");
  w5500_print_reg(w5500, cmn, w5500_gar, 4);
  printf("w5500_subr: ");
  w5500_print_reg(w5500, cmn, w5500_subr, 4);
  printf("w5500_shar: ");
  w5500_print_reg(w5500, cmn, w5500_shar, 6);
  printf("w5500_sipr: ");
  w5500_print_reg(w5500, cmn, w5500_sipr, 4);
  printf("w5500_intLevel: ");
  w5500_print_reg(w5500, cmn, w5500_intLevel, 2);
  printf("w5500_ir: ");
  w5500_print_reg(w5500, cmn, w5500_ir, 1);
  printf("w5500_imr: ");
  w5500_print_reg(w5500, cmn, w5500_imr, 1);
  printf("w5500_sir: ");
  w5500_print_reg(w5500, cmn, w5500_sir, 1);
  printf("w5500_simr: ");
  w5500_print_reg(w5500, cmn, w5500_simr, 1);
  printf("w5500_rtr: ");
  w5500_print_reg(w5500, cmn, w5500_rtr, 2);
  printf("w5500_rcr: ");
  w5500_print_reg(w5500, cmn, w5500_rcr, 1);
  printf("w5500_ptimer: ");
  w5500_print_reg(w5500, cmn, w5500_ptimer, 1);
  printf("w5500_pmagic: ");
  w5500_print_reg(w5500, cmn, w5500_pmagic, 1);
  printf("w5500_phar: ");
  w5500_print_reg(w5500, cmn, w5500_phar, 6);
  printf("w5500_psid: ");
  w5500_print_reg(w5500, cmn, w5500_psid, 2);
  printf("w5500_pmru: ");
  w5500_print_reg(w5500, cmn, w5500_pmru, 2);
  printf("w5500_uipr: ");
  w5500_print_reg(w5500, cmn, w5500_uipr, 4);
  printf("w5500_uportr: ");
  w5500_print_reg(w5500, cmn, w5500_uportr, 2);
  printf("w5500_phycfgr: ");
  w5500_print_reg(w5500, cmn, w5500_phycfgr, 1);
  printf("w5500_versionr: ");
  w5500_print_reg(w5500, cmn, w5500_versionr, 1);

  printf("w5500_socket_mr: ");
  w5500_print_reg(w5500, s1, w5500_socket_mr, 1);
  printf("w5500_socket_cr: ");
  w5500_print_reg(w5500, s1, w5500_socket_cr, 1);
  printf("w5500_socket_ir: ");
  w5500_print_reg(w5500, s1, w5500_socket_ir, 1);
  printf("w5500_socket_sr: ");
  w5500_print_reg(w5500, s1, w5500_socket_sr, 1);
  printf("w5500_socket_sport: ");
  w5500_print_reg(w5500, s1, w5500_socket_sport, 2);
  printf("w5500_socket_dhar: ");
  w5500_print_reg(w5500, s1, w5500_socket_dhar, 6);
  printf("w5500_socket_dipr: ");
  w5500_print_reg(w5500, s1, w5500_socket_dipr, 4);
  printf("w5500_socket_dport: ");
  w5500_print_reg(w5500, s1, w5500_socket_dport, 2);
  printf("w5500_socket_mssr: ");
  w5500_print_reg(w5500, s1, w5500_socket_mssr, 2);
  printf("w5500_socket_tos: ");
  w5500_print_reg(w5500, s1, w5500_socket_tos, 1);
  printf("w5500_socket_ttl: ");
  w5500_print_reg(w5500, s1, w5500_socket_ttl, 1);
  printf("w5500_socket_rxbuf_size: ");
  w5500_print_reg(w5500, s1, w5500_socket_rxbuf_size, 1);
  printf("w5500_socket_txbuf_size: ");
  w5500_print_reg(w5500, s1, w5500_socket_txbuf_size, 1);
  printf("w5500_socket_tx_fsr: ");
  w5500_print_reg(w5500, s1, w5500_socket_tx_fsr, 2);
  printf("w5500_socket_tx_rd: ");
  w5500_print_reg(w5500, s1, w5500_socket_tx_rd, 2);
  printf("w5500_socket_tx_wr: ");
  w5500_print_reg(w5500, s1, w5500_socket_tx_wr, 2);
  printf("w5500_socket_rsr: ");
  w5500_print_reg(w5500, s1, w5500_socket_rsr, 2);
  printf("w5500_socket_rx_rd: ");
  w5500_print_reg(w5500, s1, w5500_socket_rx_rd, 2);
  printf("w5500_socket_rx_wr: ");
  w5500_print_reg(w5500, s1, w5500_socket_rx_wr, 2);
  printf("w5500_socket_imr: ");
  w5500_print_reg(w5500, s1, w5500_socket_imr, 1);
  printf("w5500_socket_frag: ");
  w5500_print_reg(w5500, s1, w5500_socket_frag, 2);
  printf("w5500_socket_kpalvtr: ");
  w5500_print_reg(w5500, s1, w5500_socket_kpalvtr, 1);
}