/*
Datasheet
https://cdn.sparkfun.com/datasheets/Dev/Arduino/Shields/W5500_datasheet_v1.0.2_1.pdf
*/

#pragma once
#include <spi_device.h>
#define R 0
#define W 1

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

static const uint8_t w5500_sn_mr     = 0x00;  // Socket Mode Register
static const uint8_t w5500_sn_cr     = 0x01;  // Socket Command Register
static const uint8_t w5500_sn_ir     = 0x02;  // Socket Interrupt Register
static const uint8_t w5500_sn_status = 0x03;  // Socket Status Register
// Socket Source Port Registers
static const uint8_t w5500_sn_sport0 = 0x04;
static const uint8_t w5500_sn_sport1 = 0x05;
// Socket Destination Hardware Address Registers
static const uint8_t w5500_sn_dhar0 = 0x06;
static const uint8_t w5500_sn_dhar1 = 0x07;
static const uint8_t w5500_sn_dhar2 = 0x08;
static const uint8_t w5500_sn_dhar3 = 0x09;
static const uint8_t w5500_sn_dhar4 = 0x0A;
static const uint8_t w5500_sn_dhar5 = 0x0B;
// Socket Destination IP Address Registers
static const uint8_t w5500_sn_dipr0 = 0x0C;
static const uint8_t w5500_sn_dipr1 = 0x0D;
static const uint8_t w5500_sn_dipr2 = 0x0E;
static const uint8_t w5500_sn_dipr3 = 0x0F;
// Socket Destination Port Registers
static const uint8_t w5500_sn_dport0 = 0x10;
static const uint8_t w5500_sn_dport1 = 0x11;
// Socket Maximum Segment Size Registers
static const uint8_t w5500_sn_mssr0      = 0x12;
static const uint8_t w5500_sn_mssr1      = 0x13;
static const uint8_t w5500_sn_tos        = 0x15;  // Type of Service Register
static const uint8_t w5500_sn_ttl        = 0x16;  // Time to Live Configuration Register
static const uint8_t w5500_sn_rxbuf_size = 0x1E;  // Socket Rx Buffer Size Register
static const uint8_t w5500_sn_txbuf_size = 0x1F;  // Socket Tx Buffer Size Register
// Socket Tx Free Size Registers
static const uint8_t w5500_sn_tx_fsr0 = 0x20;
static const uint8_t w5500_sn_tx_fsr1 = 0x21;
// Socket Tx Read Pointer Registers
static const uint8_t w5500_sn_tx_rd0 = 0x22;
static const uint8_t w5500_sn_tx_rd1 = 0x23;
// Socket Tx Write Pointer Registers
static const uint8_t w5500_sn_tx_wr0 = 0x24;
static const uint8_t w5500_sn_tx_wr1 = 0x25;
// Socket Recieved Size Register
static const uint8_t w5500_sn_rsr0 = 0x26;
static const uint8_t w5500_sn_rsr1 = 0x27;
// Socket Rx Read Data Pointer Register
static const uint8_t w5500_sn_rx_rd0 = 0x28;
static const uint8_t w5500_sn_rx_rd1 = 0x29;
// Socket Rx Write Pointer Register
static const uint8_t w5500_sn_rx_wr0 = 0x2A;
static const uint8_t w5500_sn_rx_wr1 = 0x2B;
static const uint8_t w5500_sn_imr    = 0x23;  // Socket Interrupt Mask Register
// Socket Fragment Registers
static const uint8_t w5500_sn_frag0 = 0x2D;
static const uint8_t w5500_sn_frag1 = 0x2E;
static const uint8_t w5500_sn_pmru0 = 0x2F;  // Socket Keep Alive Time Register

typedef enum {
  cmn = 0x00,
  sn0 = 0x01,
  sn1 = 0x05,
  sn2 = 0x09,
  sn3 = 0x0D,
  sn4 = 0x11,
  sn5 = 0x15,
  sn6 = 0x19,
  sn7 = 0x1D,
} w5500_sn_t;

typedef enum {
  w5500_sn_closed = 0x00,
  w5500_sn_tcp    = 0x01,
  w5500_sn_udp    = 0x02,
  w5500_sn_macraw = 0x04,
} w5500_sn_mr_t;

typedef enum {
  w5500_sn_open     = 0x01,
  w5500_sn_close    = 0x10,
  w5500_sn_send     = 0x20,
  w5500_sn_send_mac = 0x21,
  w5500_sn_recv     = 0x40,
} w5500_sn_cr_t;

typedef uint8_t ip_addr_t[4];
typedef uint8_t mac_t[6];

SPI_INITFUNC(w5500);

/*Functions*/

// Read and write to single register
void w5500_rw(SPI_DEVICE_PARAM, uint8_t reg, w5500_sn_t sn, void* data, size_t len, bool rw);

// Configure w5500 with IP addr, subnet, mac addr
void w5500_configIP(SPI_DEVICE_PARAM, ip_addr_t ip, ip_addr_t gateway, ip_addr_t subnet_mask, mac_t mac);

// Configure w5500 mode register
void w5500_configMR(SPI_DEVICE_PARAM, bool wol, bool ping_block, bool pppoe, bool farp);

// Configure socket port and buffer sizes (Add buffer size checking)
void w5500_sn_config(SPI_DEVICE_PARAM, uint16_t src_port, w5500_sn_t sn, int rxbuf, int txbuf, ip_addr_t dst_ip, uint16_t dst_port);

// Configure socket destination IP and port
void w5500_sn_dst(SPI_DEVICE_PARAM, w5500_sn_t sn, ip_addr_t dst_ip, uint16_t dst_port);

// Socket mode register (protocol stuff), & command register (socket open / close)
void w5500_sn_mode(SPI_DEVICE_PARAM, w5500_sn_mr_t protocol, w5500_sn_cr_t cr, w5500_sn_t sn, bool multicast, bool unicast_block, bool broadcast_block);

// Time to live register configuration
void w5500_sn_ttl_config(SPI_DEVICE_PARAM, w5500_sn_t sn, uint8_t ttl);

// Phy config register (Ethernet speed and such)
void w5500_phy_config(SPI_DEVICE_PARAM, int speed, int duplex, int auto_negotiation);

// Check free space in TX buffer
uint16_t w5500_sn_fs_tx(SPI_DEVICE_PARAM, w5500_sn_t sn);

// Check if data is available to read
uint16_t w5500_available(SPI_DEVICE_PARAM, w5500_sn_t sn);

// write data to sn tx buffer
void w5500_sn_write(SPI_DEVICE_PARAM, w5500_sn_t sn, void* data, size_t len);

// Send data over network
void w5500_sn_transmit(SPI_DEVICE_PARAM, w5500_sn_t sn);

// Read data over spi
void w5500_read_rx(SPI_DEVICE_PARAM, w5500_sn_t sn, void* data);
