/*
Datasheet
https://cdn.sparkfun.com/datasheets/Dev/Arduino/Shields/W5500_datasheet_v1.0.2_1.pdf
*/

#pragma once
#include <spi_device.h>

#define READ 0
#define WRITE 1

//Block Select Bits


/*===============================
Common Register Addresses pg 32
===============================*/

static const uint8_t w5500_mr = 0x00;  // Mode Register
// Gateway IP Address Registers
static const uint8_t w5500_gar0 = 0x01;
static const uint8_t w5500_gar1 = 0x02;
static const uint8_t w5500_gar2 = 0x03;
static const uint8_t w5500_gar3 = 0x04;
// Subnet Mask Registers
static const uint8_t w5500_subr0 = 0x05;
static const uint8_t w5500_subr1 = 0x06;
static const uint8_t w5500_subr2 = 0x07;
static const uint8_t w5500_subr3 = 0x08;
// Source Hardware Address Registers
static const uint8_t w5500_shar0 = 0x09;
static const uint8_t w5500_shar1 = 0x0A;
static const uint8_t w5500_shar2 = 0x0B;
static const uint8_t w5500_shar3 = 0x0C;
static const uint8_t w5500_shar4 = 0x0D;
static const uint8_t w5500_shar5 = 0x0E;
// Source IP Address Registers
static const uint8_t w5500_sipr0 = 0x0F;
static const uint8_t w5500_sipr1 = 0x10;
static const uint8_t w5500_sipr2 = 0x11;
static const uint8_t w5500_sipr4 = 0x12;
// Interrupt Level Address Registers
static const uint8_t w5500_intLevel0 = 0x13;
static const uint8_t w5500_intLevel1 = 0x14;
static const uint8_t w5500_ir        = 0x15;  // Interrupt Register
static const uint8_t w5500_imr       = 0x16;  // Interrupt Mask Register
static const uint8_t w5500_sir       = 0x17;  // Socket Interrupt Register
static const uint8_t w5500_simr      = 0x18;  // Socket Interrupt Mask Register
// Retry Time Value Registers
static const uint8_t w5500_rtr0   = 0x19;
static const uint8_t w5500_rtr1   = 0x1A;
static const uint8_t w5500_rcr    = 0x1B;  // Retry Count Register
static const uint8_t w5500_ptimer = 0x1C;  // PPP Link Control Protocol Request Timer Register
static const uint8_t w5500_pmagic = 0x1D;  // PPP Link Control Protocol Magic Number Register
// Destination Hardware Address Registers (PPPoE Mode)
static const uint8_t w5500_phar0 = 0x1E;
static const uint8_t w5500_phar1 = 0x1F;
static const uint8_t w5500_phar2 = 0x20;
static const uint8_t w5500_phar3 = 0x21;
static const uint8_t w5500_phar4 = 0x22;
static const uint8_t w5500_phar5 = 0x23;
// Session ID Registers (PPPoE Mode)
static const uint8_t w5500_psid0 = 0x24;
static const uint8_t w5500_psid1 = 0x25;
// Maximum Recieve Unit Registers (PPPoE Mode)
static const uint8_t w5500_pmru0 = 0x26;
static const uint8_t w5500_pmru1 = 0x27;
// Unreachable IP Address Registers
static const uint8_t w5500_uipr0 = 0x28;
static const uint8_t w5500_uipr1 = 0x29;
static const uint8_t w5500_uipr2 = 0x2A;
static const uint8_t w5500_uipr3 = 0x2B;
// Unreachable Port Registers
static const uint8_t w5500_uportr0  = 0x2C;
static const uint8_t w5500_uportr1  = 0x2D;
static const uint8_t w5500_phycfgr  = 0x2E;  // PHY Configuration Register
static const uint8_t w5500_versionr = 0x39;  // Chip Version Register

/*===============================
Socket Register Addresses pg 44
===============================*/

static const uint8_t w5500_socket_mr     = 0x00;  // Socket Mode Register
static const uint8_t w5500_socket_cr     = 0x01;  // Socket Command Register
static const uint8_t w5500_socket_ir     = 0x02;  // Socket Interrupt Register
static const uint8_t w5500_socket_status = 0x03;  // Socket Status Register
// Socket Source Port Registers
static const uint8_t w5500_socket_sport0 = 0x04;
static const uint8_t w5500_socket_sport1 = 0x05;
// Socket Destination Hardware Address Registers
static const uint8_t w5500_socket_dhar0 = 0x06;
static const uint8_t w5500_socket_dhar1 = 0x07;
static const uint8_t w5500_socket_dhar2 = 0x08;
static const uint8_t w5500_socket_dhar3 = 0x09;
static const uint8_t w5500_socket_dhar4 = 0x0A;
static const uint8_t w5500_socket_dhar5 = 0x0B;
// Socket Destination IP Address Registers
static const uint8_t w5500_socket_dipr0 = 0x0C;
static const uint8_t w5500_socket_dipr1 = 0x0D;
static const uint8_t w5500_socket_dipr2 = 0x0E;
static const uint8_t w5500_socket_dipr3 = 0x0F;
// Socket Destination Port Registers
static const uint8_t w5500_socket_dport0 = 0x10;
static const uint8_t w5500_socket_dport1 = 0x11;
// Socket Maximum Segment Size Registers
static const uint8_t w5500_socket_mssr0     = 0x12;
static const uint8_t w5500_socket_mssr1     = 0x13;
static const uint8_t w5500_socket_tos       = 0x15;  // Type of Service Register
static const uint8_t w5500_socket_ttl       = 0x16;  // Time to Live Configuration Register
static const uint8_t w5500_socket_rbuf_size = 0x1E;  // Socket Rx Buffer Size Register
static const uint8_t w5500_socket_tbuf_size = 0x1F;  // Socket Tx Buffer Size Register
// Socket Tx Free Size Registers
static const uint8_t w5500_socket_tx_fsr0 = 0x20;
static const uint8_t w5500_socket_tx_fsr1 = 0x21;
// Socket Tx Read Pointer Registers
static const uint8_t w5500_socket_tx_rd0 = 0x22;
static const uint8_t w5500_socket_tx_rd1 = 0x23;
// Socket Tx Write Pointer Registers
static const uint8_t w5500_socket_tx_wr0 = 0x24;
static const uint8_t w5500_socket_tx_wr1 = 0x25;
// Socket Recieved Size Register
static const uint8_t w5500_socket_rsr0 = 0x26;
static const uint8_t w5500_socket_rsr1 = 0x27;
// Socket Rx Read Data Pointer Register
static const uint8_t w5500_socket_rx_rd0 = 0x28;
static const uint8_t w5500_socket_rx_rd1 = 0x29;
// Socket Rx Write Pointer Register
static const uint8_t w5500_socket_rx_wr0 = 0x2A;
static const uint8_t w5500_socket_rx_wr1 = 0x2B;
static const uint8_t w5500_socket_imr    = 0x23;  // Socket Interrupt Mask Register
// Socket Fragment Registers
static const uint8_t w5500_socket_frag0 = 0x2D;
static const uint8_t w5500_socket_frag1 = 0x2E;
static const uint8_t w5500_socket_pmru0 = 0x2F;  // Socket Keep Alive Time Register

typedef enum {
 common = 0x00,
 socket0 =  0x01,  
 socket0_tx  = 0x02,
 socket0_rx  = 0x03,

 socket1 = 0x05,
 socket1_tx = 0x06,
 socket1_rx = 0x07,

 socket2 = 0x09,
 socket2_tx = 0x0A,
 socket2_rx = 0x0B,

 socket3 = 0x0D,
 socket3_tx = 0x0E,
 socket3_rx = 0x0F,

 socket4 = 0x11,
 socket4_tx = 0x12,
 socket4_rx = 0x13,

 socket5 = 0x15, 
 socket5_tx = 0x16,
 socket5_rx = 0x17,

 socket6 = 0x19,  
 socket6_tx = 0x1A,
 socket6_rx = 0x1B,

 socket7 = 0x1D,  
 socket7_tx = 0x1E,
 socket7_rx = 0x1F,
} block_select_t;

typedef enum {
  w5500_socket_closed = 0x00,
  w5500_socket_tcp = 0x01,
  w5500_socket_udp = 0x02,
  w5500_socket_macraw = 0x04,
} w5500_socket_mode_t;

typedef enum {
  w5500_socket_open = 0x01,
  w5500_socket_close = 0x10,
  w5500_socket_send = 0x20,
  w5500_socket_send_mac = 0x21,
  w5500_socket_recv = 0x40

} w5500_socket_command_t;

typedef enum {
   one = 0x01,
   two = 0x02,
   four = 0x04,
   eight = 0x08,
   sixteen = 0x16,
} buf_size_t;



typedef uint8_t ip_addr_t[4];

SPI_INITFUNC(w5500);

/*Functions*/

void w5500_wreg(SPI_DEVICE_PARAM, uint8_t reg, block_select_t bsb, void* data, size_t len);
void w5500_rreg(SPI_DEVICE_PARAM, uint8_t reg, block_select_t bsb, void* data, size_t len);
void w5500_config(SPI_DEVICE_PARAM, ip_addr_t ip, ip_addr_t gateway, ip_addr_t subnet_mask);
//config socket ports and buffers
void w5500_config_socket( SPI_DEVICE_PARAM, uint8_t src_port[2], block_select_t bsb, 
buf_size_t* rxbuf, buf_size_t* txbuf ,
ip_addr_t dst_ip, uint8_t dst_port[2]);

//config socket mode and command registers
void w5500_socket_mode(w5500_socket_mode_t mode, w5500_socket_command_t command, 
block_select_t socket, bool multicast, bool broadcast_block, bool igmp, bool unicast_block
);
void w5500_set_wol(SPI_DEVICE_PARAM, bool wol);     
uint16_t w5500_free(SPI_DEVICE_PARAM, block_select_t bsb);
uint16_t w5500_available(SPI_DEVICE_PARAM, block_select_t bsb);  // checks if data is available                                             // Enable or disable wake on lan


// void w5500_transmit(SPI_DEVICE_PARAM, w5500_socket_t socket);                       // sends all data in tx buffer of socket over ethernet




