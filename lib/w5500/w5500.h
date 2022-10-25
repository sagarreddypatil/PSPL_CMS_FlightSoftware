/*
Datasheet
https://cdn.sparkfun.com/datasheets/Dev/Arduino/Shields/W5500_datasheet_v1.0.2_1.pdf
*/

#pragma once
#include <spi_device.h>

//Block Select Bits
#define COMMON 0b00000
#define SOCKET0 0b00001 
#define SOCKET0_TX 0b00010
#define SOCKET0_RX 0b00011

#define SOCKET1 0b00110
#define SOCKET1_TX 0b00111
#define SOCKET1_RX 0b01000

#define SOCKET2 0b01010
#define SOCKET2_TX 0b01011
#define SOCKET2_RX 0b01100

#define SOCKET3 0b01110
#define SOCKET3_TX 0b01111
#define SOCKET3_RX 0b10000

#define SOCKET4 0b10010
#define SOCKET4_TX 0b10011
#define SOCKET4_RX 0b10100

#define SOCKET5 0b10101 
#define SOCKET5_TX 0b10110
#define SOCKET5_RX 0b10111

#define SOCKET6 0b11001  
#define SOCKET6_TX 0b11010
#define SOCKET6_RX 0b11011

#define SOCKET7 0b11101  
#define SOCKET7_TX 0b11110
#define SOCKET7_RX 0b11111

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
  w5500_socket0,
  w5500_socket1,
  w5500_socket2,
  w5500_socket3,
  w5500_socket4,
  w5500_socket5,
  w5500_socket6,
  w5500_socket7,
} w5500_socket_t;

typedef enum {
  w5500_socket_closed,
  w5500_socket_tcp,
  w5500_socket_udp,
  w5500_socket_macraw,
} w5500_socket_mode_t;

typedef uint8_t ip_addr_t[4];

SPI_INITFUNC(w5500);

/*Functions*/

uint8_t w5500_rreg_byte(SPI_DEVICE_PARAM, uint8_t reg);  // Read from single register
void w5500_wreg_byte(SPI_DEVICE_PARAM, uint8_t reg);     // Write to single register

void ws5500_config(SPI_DEVICE_PARAM, ip_addr_t ip, ip_addr_t gateway, ip_addr_t subnet_mask);  // Set source ip address of ws5500
void w5500_set_wol(SPI_DEVICE_PARAM, bool wol);                                                // Enable or disable wake on lan
void ws5500_config_socket(SPI_DEVICE_PARAM,
                          w5500_socket_t socket, w5500_socket_mode_t mode,  // Set socket mode
                          uint16_t src_port,                                // Set source port
                          ip_addr_t dst_ip, uint16_t dst_port,              // Set destination ip and port
                          bool multicast);                                  // Enable or disable multicast, uses destination ip as multicast ip

void w5500_write(SPI_DEVICE_PARAM, w5500_socket_t socket, void* data, size_t len);  // writes to tx buffer of socket
uint16_t w5500_free(SPI_DEVICE_PARAM, w5500_socket_t socket);                       // returns free space in tx buffer of socket
void w5500_transmit(SPI_DEVICE_PARAM, w5500_socket_t socket);                       // sends all data in tx buffer of socket over ethernet

uint16_t w5500_available(SPI_DEVICE_PARAM, w5500_socket_t socket);  // checks if data is available
void w5500_read(SPI_DEVICE_PARAM, w5500_socket_t socket);           // reads in all data from rx buffer of socket
