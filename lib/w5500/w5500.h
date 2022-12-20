/*
Datasheet
https://cdn.sparkfun.com/datasheets/Dev/Arduino/Shields/W5500_datasheet_v1.0.2_1.pdf
*/

#pragma once
#include <spi_device.h>

/*===============================
Common Register Addresses pg 32
===============================*/

static const uint16_t w5500_mr = 0x00;  // Mode Register
static const uint16_t w5500_gar = 0x01; // Gateway IP Address Register
static const uint16_t w5500_subr = 0x05;// Subnet Mask Register
static const uint16_t w5500_shar = 0x09;// Source Hardware Address Register
static const uint16_t w5500_sipr = 0x0F;// Source IP Address Register
static const uint16_t w5500_intLevel = 0x13;// Interrupt Level Address Registers
static const uint16_t w5500_ir        = 0x15;  // Interrupt Register
static const uint16_t w5500_imr       = 0x16;  // Interrupt Mask Register
static const uint16_t w5500_sir       = 0x17;  // Socket Interrupt Register
static const uint16_t w5500_simr      = 0x18;  // Socket Interrupt Mask Register
static const uint16_t w5500_rtr   = 0x19; // Retry Time Value Register
static const uint16_t w5500_rcr    = 0x1B;  // Retry Count Register
static const uint16_t w5500_ptimer = 0x1C;  // PPP Link Control Protocol Request Timer Register
static const uint16_t w5500_pmagic = 0x1D;  // PPP Link Control Protocol Magic Number Register
static const uint16_t w5500_phar = 0x1E; // Destination Hardware Address Registers (PPPoE Mode)
static const uint16_t w5500_psid = 0x24; // Session ID Registers (PPPoE Mode)
static const uint16_t w5500_pmru = 0x26; // Maximum Recieve Unit Registers (PPPoE Mode)
static const uint16_t w5500_uipr = 0x28; // Unreachable IP Address Register
static const uint16_t w5500_uportr  = 0x2C; // Unreachable Port Registers
static const uint16_t w5500_phycfgr  = 0x2E;  // PHY Configuration Register
static const uint16_t w5500_versionr = 0x39;  // Chip Version Register

/*===============================
Socket Register Addresses pg 44
===============================*/

static const uint16_t w5500_s_mr     = 0x00;  // Socket Mode Register
static const uint16_t w5500_s_cr     = 0x01;  // Socket Command Register
static const uint16_t w5500_s_ir     = 0x02;  // Socket Interrupt Register
static const uint16_t w5500_s_status = 0x03;  // Socket Status Register
static const uint16_t w5500_s_sport = 0x04; // Socket Source Port Register
static const uint16_t w5500_s_dhar = 0x06; // Socket Destination Hardware Address Registers
static const uint16_t w5500_s_dipr = 0x0C; // Socket Destination IP Address Register
static const uint16_t w5500_s_dport = 0x10; // Socket Destination Port Register
static const uint16_t w5500_s_mssr      = 0x12; // Socket Maximum Segment Size Register
static const uint16_t w5500_s_tos        = 0x15;  // Type of Service Register
static const uint16_t w5500_s_ttl        = 0x16;  // Time to Live Configuration Register
static const uint16_t w5500_s_rxbuf_size = 0x1E;  // Socket Rx Buffer Size Register
static const uint16_t w5500_s_txbuf_size = 0x1F;  // Socket Tx Buffer Size Register
static const uint16_t w5500_s_tx_fsr = 0x20; // Socket Tx Free Size Register
static const uint16_t w5500_s_tx_rd = 0x22; // Socket Tx Read Pointer Register
static const uint16_t w5500_s_tx_wr = 0x24; // Socket Tx Write Pointer Register
static const uint16_t w5500_s_rsr = 0x26; // Socket Recieved Size Register
static const uint16_t w5500_s_rx_rd = 0x28; // Socket Rx Read Data Pointer Register
static const uint16_t w5500_s_rx_wr = 0x2A; // Socket Rx Write Pointer Register
static const uint16_t w5500_s_imr    = 0x23;  // Socket Interrupt Mask Register
static const uint16_t w5500_s_frag = 0x2D; // Socket Fragment Register

typedef enum {
  cmn = 0b00000,
  s0 = 0b00001,
  s1 = 0b00101,
  s2 = 0b01001,
  s3 = 0b01101,
  s4 = 0b10001,
  s5 = 0b10101,
  s6 = 0b11001,
  s7 = 0b11101,
} w5500_s_t;

typedef enum {
  w5500_s_closed = 0x00,
  w5500_s_tcp    = 0x01,
  w5500_s_udp    = 0x02,
  w5500_s_macraw = 0x04,
} w5500_protocol_t;

typedef enum {
  w5500_s_open     = 0x01,
  w5500_s_close    = 0x10,
  w5500_s_send     = 0x20,
  w5500_s_send_mac = 0x21,
  w5500_s_recv     = 0x40,
} w5500_mode_t;

typedef uint8_t ip_t[4];
typedef uint8_t mac_t[6];

SPI_INITFUNC(w5500);

/*Functions*/

// void w5500_rw(SPI_DEVICE_PARAM, uint16_t reg, w5500_s_t s, void* data, size_t len, bool write);                                     // Read and write to single register
// void w5500_configIP(SPI_DEVICE_PARAM, ip_addr_t ip, ip_addr_t gateway, ip_addr_t subnet_mask, mac_t mac);                             // Configure w5500 with IP addr, subnet, mac addr
// void w5500_configMR(SPI_DEVICE_PARAM, bool wol, bool ping_block, bool pppoe, bool farp);                                              // Configure w5500 mode register
// void w5500_s_config(SPI_DEVICE_PARAM, uint16_t src_port, w5500_s_t s, int rxbuf, int txbuf, ip_addr_t dst_ip, uint16_t dst_port);  // Configure socket port and buffer sizes (Add buffer size checking)
// void w5500_s_mode(SPI_DEVICE_PARAM, w5500_s_mr_t protocol, w5500_s_cr_t cr, w5500_s_t s,                                         // Socket mode register (protocol stuff), & command register (socket open / close)
//                    bool multicast, bool unicast_block, bool broadcast_block);                                                         
// void w5500_s_ttl_config(SPI_DEVICE_PARAM, w5500_s_t s, uint8_t ttl);                                                               // Time to live register configuration
// void w5500_phy_config(SPI_DEVICE_PARAM, int speed, int duplex, int auto_negotiation);                                                 // Phy config register (Ethernet speed and such)
// uint16_t w5500_s_fs_tx(SPI_DEVICE_PARAM, w5500_s_t s);                                                                             // Check free space in TX buffer 
// uint16_t w5500_available(SPI_DEVICE_PARAM, w5500_s_t s);                                                                            // Check if data is available to read
// void w5500_s_write(SPI_DEVICE_PARAM, w5500_s_t s, void* data, size_t len);                                                         // write data to s tx buffer
// void w5500_s_transmit(SPI_DEVICE_PARAM, w5500_s_t s);                                                                              // Send data over network
// void w5500_read_rx(SPI_DEVICE_PARAM, w5500_s_t s, void* data);                                                                      // Read data over spi

void w5500_init(SPI_DEVICE_PARAM, ip_t gateway, ip_t sub_mask, ip_t src_ip, mac_t mac_addr);
void w5500_close(SPI_DEVICE_PARAM, w5500_s_t s);
void w5500_open(SPI_DEVICE_PARAM, w5500_s_t s);
void w5500_status(SPI_DEVICE_PARAM);
//void w5500_s_init(SPI_DEVICE_PARAM, w5500_s_t sn, ip_t dst_ip, uint16_t src_port, uint16_t dst_port);
//void w5500_config(bool wol, bool ping_block, bool pppoe, bool farp);
//void w5500_s_config(SPI_DEVICE_PARAM, w5500_s_t sn, w5500_protocol_t protocol, bool multicast, bool unicast_block, bool broadcast_block);
//void w5500_set_dst();
