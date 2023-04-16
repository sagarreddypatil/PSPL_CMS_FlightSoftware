/** @file w5500.h
 *  @brief function prototypes for the w5500 driver
 *
 * @author Jay S. Jagani (jjagani)
 *
 */

#pragma once
#include <spi_device.h>

/* Common Register Addresses pg 32 */

static const uint16_t W5500_MR        = 0x00;  // Mode
static const uint16_t W5500_GAR0      = 0x01;  // Gateway IP Address
static const uint16_t W5500_SUBR0     = 0x05;  // Subnet Mask
static const uint16_t W5500_SHAR0     = 0x09;  // Source Hardware Address
static const uint16_t W5500_SIPR0     = 0x0F;  // Source IP Address
static const uint16_t W5500_INTLEVEL0 = 0x13;  // Interrupt Low Level Timer
static const uint16_t W5500_IR        = 0x15;  // Interrupt
static const uint16_t W5500_IMR       = 0x16;  // Interrupt Mask
static const uint16_t W5500_SIR       = 0x17;  // Socket Interrupt
static const uint16_t W5500_SIMR      = 0x18;  // Socket Interrupt Mask
static const uint16_t W5500_RTR0      = 0x19;  // Retry time
static const uint16_t W5500_RCR       = 0x1B;  // Retry count
static const uint16_t W5500_PTIMER    = 0x1C;  // PPP LCP Request Timer
static const uint16_t W5500_PMAGIC    = 0x1D;  // PPP LCP Magic number
static const uint16_t W5500_PHAR0     = 0x1E;  // Destination Hardware Address (PPPoE mode)
static const uint16_t W5500_PSID0     = 0x24;  // Session ID (PPPoE mode)
static const uint16_t W5500_PMRU0     = 0x26;  // Maximum Segment Size (PPPoE mode)
static const uint16_t W5500_UIPR0     = 0x28;  // Unreachable IP address
static const uint16_t W5500_UPORTR0   = 0x2C;  // Unreachable Port
static const uint16_t W5500_PHYCFGR   = 0x2E;  // PHY Configuration
static const uint16_t W5500_VERSIONR  = 0x39;  // Chip Version

/* Socket Register Addresses pg 44 */

static const uint16_t W5500_Sn_MR         = 0x00;  // Socket n Mode
static const uint16_t W5500_Sn_CR         = 0x01;  // Socket n Command
static const uint16_t W5500_Sn_IR         = 0x02;  // Socket n Interrupt
static const uint16_t W5500_Sn_SR         = 0x03;  // Socket n Status
static const uint16_t W5500_Sn_PORT0      = 0x04;  // Socket n Source Port
static const uint16_t W5500_Sn_DHAR0      = 0x06;  // Socket n Destination Hardware Address
static const uint16_t W5500_Sn_DIPR0      = 0x0C;  // Socket n Destination IP Address
static const uint16_t W5500_Sn_DPORT0     = 0x10;  // Socket n Destination Port
static const uint16_t W5500_Sn_MSSR0      = 0x12;  // Socket n Maximum Segment Size
static const uint16_t W5500_Sn_TOS        = 0x15;  // Socket n Type of Service
static const uint16_t W5500_Sn_TTL        = 0x16;  // Socket n Time to Live
static const uint16_t W5500_Sn_RXBUF_SIZE = 0x1E;  // Socket n Receive Buffer Size
static const uint16_t W5500_Sn_TXBUF_SIZE = 0x1F;  // Socket n Transmit Buffer Size
static const uint16_t W5500_Sn_TX_FSR0    = 0x20;  // Socket n Transmit Free Size
static const uint16_t W5500_Sn_TX_RD0     = 0x22;  // Socket n Transmit Read Pointer
static const uint16_t W5500_Sn_TX_WR0     = 0x24;  // Socket n Transmit Write Pointer
static const uint16_t W5500_Sn_RX_RSR0    = 0x26;  // Socket n Received Size
static const uint16_t W5500_Sn_RX_RD0     = 0x28;  // Socket n Received Read Pointer
static const uint16_t W5500_Sn_RX_WR0     = 0x2A;  // Socket n Received Write Pointer
static const uint16_t W5500_Sn_IMR        = 0x2C;  // Socket n Interrupt Mask
static const uint16_t W5500_Sn_FRAG0      = 0x2D;  // Socket n Fragment
static const uint16_t W5500_Sn_KPALVTR    = 0x2F;  // Socket n Keep Alive Timer

/* Interrupts */
typedef union {
  struct {
    uint8_t CON : 1;
    uint8_t DISCON : 1;
    uint8_t RECV : 1;
    uint8_t TIMEOUT : 1;
    uint8_t SEND_OK : 1;
    uint8_t : 3;
  };
  uint8_t value;
} w5500_socket_interrupt_t;

/*Socket Commands*/

typedef enum {
  W5500_CMD_OPEN      = 0x01,
  W5500_CMD_LISTEN    = 0x02,
  W5500_CMD_CONNECT   = 0x04,
  W5500_CMD_DISCON    = 0x08,
  W5500_CMD_CLOSE     = 0x10,
  W5500_CMD_SEND      = 0x20,
  W5500_CMD_SEND_MAC  = 0x21,
  W5500_CMD_SEND_KEEP = 0x22,
  W5500_CMD_RECV      = 0x40,
} w5500_socket_command_t;

typedef enum {
  W5500_SOCK_CLOSED      = 0x00,
  W5500_SOCK_INIT        = 0x13,
  W5500_SOCK_LISTEN      = 0x14,
  W5500_SOCK_ESTABLISHED = 0x17,
  W5500_SOCK_CLOSE_WAIT  = 0x1C,
  W5500_SOCK_UDP         = 0x22,
  W5500_SOCK_MACRAW      = 0x42,
  W5500_SOCK_SYNSENT     = 0x15,
  W5500_SOCK_SYNRECV     = 0x16,
  W5500_SOCK_FIN_WAIT    = 0x18,
  W5500_SOCK_CLOSING     = 0x1A,
  W5500_SOCK_TIME_WAIT   = 0x1B,
  W5500_SOCK_LAST_ACK    = 0x1D,
} w5500_socket_status_t;

/* Socket numbers */

typedef enum {
  W5500_COMMON = 0b00000,
  W5500_S0     = 0b00001,
  W5500_S1     = 0b00101,
  W5500_S2     = 0b01001,
  W5500_S3     = 0b01101,
  W5500_S4     = 0b10001,
  W5500_S5     = 0b10101,
  W5500_S6     = 0b11001,
  W5500_S7     = 0b11101,
} w5500_socket_t;

/*Errors*/
typedef enum {
  SUCCESS              = 0,
  ERR_SOCK_CLOSE       = -1,
  ERR_INVALID_PROTOCOL = -2,
  ERR_INIT_UDP         = -3,
  ERR_INIT_TCP         = -4,
  ERR_INIT_SERVER      = -5,
  ERR_CONNECT_SERVER   = -6,
  ERR_INSUFFICIENT_MEM = -7,
} w5500_error_t;

/* Protocols for sockets */

typedef enum {
  W5500_PROTOCOL_CLOSED = 0,
  W5500_PROTOCOL_TCP    = 0b1,
  W5500_PROTOCOL_UDP    = 0b10,
  W5500_PROTOCOL_MACRAW = 0b100
} w5500_protocol_t;

typedef uint8_t ip_t[4];
typedef uint8_t mac_t[6];

SPI_INITFUNC(w5500);

/* The new era of ethernet driver */

/*Register Read and Write*/
void w5500_rw(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg, bool write, void* data, size_t len);
static inline uint8_t w5500_read8(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg) {
  uint8_t data;
  w5500_rw(spi, s, reg, false, &data, 1);
  return data;
}
static inline void w5500_write8(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg, uint8_t data) {
  w5500_rw(spi, s, reg, true, &data, 1);
}
static inline uint16_t w5500_read16(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg) {
  uint8_t buf[2];
  w5500_rw(spi, s, reg, false, buf, 2);
  return ((uint16_t)(buf[0]) << 8) | buf[1];
}
static inline void w5500_write16(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg, uint16_t data) {
  uint8_t buf[2] = {data >> 8, data & 0xFF};
  w5500_rw(spi, s, reg, true, buf, 2);
}
static inline void w5500_set_flag(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg, uint8_t bit, bool value) {
  w5500_write8(spi, s, reg, w5500_read8(spi, s, reg) | ((value & 1) << bit));
}

/*W5500 Constructors*/
void w5500_init(SPI_DEVICE_PARAM);
bool w5500_ready(SPI_DEVICE_PARAM);
void w5500_config(SPI_DEVICE_PARAM, mac_t src_mac, ip_t src_ip, ip_t subnet_mask, ip_t gateway);
void w5500_set_wol(SPI_DEVICE_PARAM, bool wol);
void w5500_set_ping_block(SPI_DEVICE_PARAM, bool block_ping);
void w5500_set_force_arp(SPI_DEVICE_PARAM, bool force_arp);

/*Socket Constructors*/
w5500_error_t w5500_create_udp_socket(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t src_port, bool multicast, bool block_broadcast, bool block_unicast);
w5500_error_t w5500_create_tcp_socket(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t src_port);

void w5500_wait_socket_status(SPI_DEVICE_PARAM, w5500_socket_t s, w5500_socket_status_t status);

/*Writing and Reading for Data Transfer*/
void w5500_read_data(SPI_DEVICE_PARAM, w5500_socket_t s, uint8_t* data, size_t len);
w5500_error_t w5500_write_data(SPI_DEVICE_PARAM, w5500_socket_t s, bool keep_alive, void* data, size_t len);  // keep alive is for TCP only

/* Sends commands to w5500*/
w5500_error_t w5500_command(SPI_DEVICE_PARAM, w5500_socket_t s, w5500_socket_command_t command);

void w5500_print_reg(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg, uint8_t len);
void w5500_print_all(SPI_DEVICE_PARAM, w5500_socket_t s);
