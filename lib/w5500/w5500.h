/** @file w5500.h
 *  @brief function prototypes for the w5500 driver
 *
 * @author Jay S. Jagani (jjagani)
 *
 */

#pragma once
#include <spi_device.h>

/* Common Register Addresses pg 32 */

static const uint16_t w5500_mr       = 0x00;  // Mode Register
static const uint16_t w5500_gar      = 0x01;  // Gateway IP Address Register
static const uint16_t w5500_subr     = 0x05;  // Subnet Mask Register
static const uint16_t w5500_shar     = 0x09;  // Source Hardware Address Register
static const uint16_t w5500_sipr     = 0x0F;  // Source IP Address Register
static const uint16_t w5500_intLevel = 0x13;  // Interrupt Level Address Registers
static const uint16_t w5500_ir       = 0x15;  // Interrupt Register
static const uint16_t w5500_imr      = 0x16;  // Interrupt Mask Register
static const uint16_t w5500_sir      = 0x17;  // Socket Interrupt Register
static const uint16_t w5500_simr     = 0x18;  // Socket Interrupt Mask Register
static const uint16_t w5500_rtr      = 0x19;  // Retry Time Value Register
static const uint16_t w5500_rcr      = 0x1B;  // Retry Count Register
static const uint16_t w5500_ptimer   = 0x1C;  // PPP Link Control Protocol Request Timer Register
static const uint16_t w5500_pmagic   = 0x1D;  // PPP Link Control Protocol Magic Number Register
static const uint16_t w5500_phar     = 0x1E;  // Destination Hardware Address Registers (PPPoE Mode)
static const uint16_t w5500_psid     = 0x24;  // Session ID Registers (PPPoE Mode)
static const uint16_t w5500_pmru     = 0x26;  // Maximum Recieve Unit Registers (PPPoE Mode)
static const uint16_t w5500_uipr     = 0x28;  // Unreachable IP Address Register
static const uint16_t w5500_uportr   = 0x2C;  // Unreachable Port Registers
static const uint16_t w5500_phycfgr  = 0x2E;  // PHY Configuration Register
static const uint16_t w5500_versionr = 0x39;  // Chip Version Register

/* Socket Register Addresses pg 44 */

static const uint16_t w5500_socket_mr         = 0x00;  // Socket Mode Register
static const uint16_t w5500_socket_cr         = 0x01;  // Socket Command Register
static const uint16_t w5500_socket_ir         = 0x02;  // Socket Interrupt Register
static const uint16_t w5500_socket_sr         = 0x03;  // Socket Status Register
static const uint16_t w5500_socket_sport      = 0x04;  // Socket Source Port Register
static const uint16_t w5500_socket_dhar       = 0x06;  // Socket Destination Hardware Address Registers
static const uint16_t w5500_socket_dipr       = 0x0C;  // Socket Destination IP Address Register
static const uint16_t w5500_socket_dport      = 0x10;  // Socket Destination Port Register
static const uint16_t w5500_socket_mssr       = 0x12;  // Socket Maximum Segment Size Register
static const uint16_t w5500_socket_tos        = 0x15;  // Type of Service Register
static const uint16_t w5500_socket_ttl        = 0x16;  // Time to Live Configuration Register
static const uint16_t w5500_socket_rxbuf_size = 0x1E;  // Socket Rx Buffer Size Register
static const uint16_t w5500_socket_txbuf_size = 0x1F;  // Socket Tx Buffer Size Register
static const uint16_t w5500_socket_tx_fsr     = 0x20;  // Socket Tx Free Size Register
static const uint16_t w5500_socket_tx_rd      = 0x22;  // Socket Tx Read Pointer Register
static const uint16_t w5500_socket_tx_wr      = 0x24;  // Socket Tx Write Pointer Register
static const uint16_t w5500_socket_rsr        = 0x26;  // Socket Recieved Size Register
static const uint16_t w5500_socket_rx_rd      = 0x28;  // Socket Rx Read Data Pointer Register
static const uint16_t w5500_socket_rx_wr      = 0x2A;  // Socket Rx Write Pointer Register
static const uint16_t w5500_socket_imr        = 0x23;  // Socket Interrupt Mask Register
static const uint16_t w5500_socket_frag       = 0x2D;  // Socket Fragment Register
static const uint16_t w5500_socket_kpalvtr    = 0x2F;  // Socket Keep alive Time Register

/* Interrupts */
static const uint8_t INT_SENDOK     = 0x10;
static const uint8_t INT_TIMEOUT    = 0x08;
static const uint8_t INT_RECV       = 0x04;
static const uint8_t INT_DISCONNECT = 0x02;
static const uint8_t INT_CONNECT    = 0x00;

/*Socket Commands*/
static const uint8_t CMD_OPEN       = 0x01;
static const uint8_t CMD_LISTEN     = 0x02;
static const uint8_t CMD_CONNECT    = 0x04;
static const uint8_t CMD_DISCONNECT = 0x08;
static const uint8_t CMD_CLOSE      = 0x10;
static const uint8_t CMD_SEND       = 0x20;
static const uint8_t CMD_SEND_MAC   = 0x21;
static const uint8_t CMD_SEND_KEEP  = 0x22;
static const uint8_t CMD_RECIEVE    = 0x40;

/*Socket Status*/

static const uint8_t SOCK_CLOSED      = 0x00;
static const uint8_t SOCK_INIT        = 0x13;
static const uint8_t SOCK_LISTEN      = 0x14;
static const uint8_t SOCK_ESTABLISHED = 0x17;
static const uint8_t SOCK_CLOSE_WAIT  = 0x1C;
static const uint8_t SOCK_UDP         = 0x22;
static const uint8_t SOCK_MACRAW      = 0x42;
static const uint8_t SOCK_SYNSENT     = 0x15;
static const uint8_t SOCK_SYNRECV     = 0x16;
static const uint8_t SOCK_FIN_WAIT    = 0x18;
static const uint8_t SOCK_CLOSING     = 0x1A;
static const uint8_t SOCK_TIME_WAIT   = 0x1B;
static const uint8_t SOCK_LAST_ACK    = 0x1D;

/* Socket numbers */

typedef enum {
  cmn = 0x00,
  s0  = 0x01,
  s1  = 0x05,
  s2  = 0x00,
  s3  = 0x0D,
  s4  = 0x11,
  s5  = 0x15,
  s6  = 0x19,
  s7  = 0x1E,
} w5500_socket_t;

/* Protocols for sockets */

typedef enum {
  tcp = 0x01,
  udp = 0x02,
} w5500_protocol_t;

typedef uint8_t ip_t[4];
typedef uint8_t mac_t[6];

SPI_INITFUNC(w5500);

/* The new era of ethernet driver */

/*W5500 Constructors*/
uint8_t w5500_create(SPI_DEVICE_PARAM, ip_t src_ip, mac_t src_mac, ip_t subnet_mask, ip_t gateway, uint8_t retry_time, uint8_t retry_count, bool wake_on_LAN, bool block_ping, bool force_ARP);

/*Socket Constructors*/
uint8_t w5500_create_socket(SPI_DEVICE_PARAM, w5500_socket_t s, w5500_protocol_t protocol, bool multicast, ip_t dst_addr, uint16_t src_port, uint16_t dst_port, uint8_t rx_buffer_size, uint8_t tx_buffer_size, bool block_broadcast, bool block_unicast);

/*Socket Writing & Reading*/
uint8_t w5500_write_data(SPI_DEVICE_PARAM, w5500_socket_t s, void* data, size_t length);
uint8_t w5500_read_data(SPI_DEVICE_PARAM, w5500_socket_t s, void* recv, size_t length);

/*Socket Commands*/

uint8_t w5500_command(SPI_DEVICE_PARAM, w5500_socket_t s, uint8_t command);
uint8_t w5500_open(SPI_DEVICE_PARAM, w5500_socket_t s);
uint8_t w5500_listen(SPI_DEVICE_PARAM, w5500_socket_t s);
uint8_t w5500_connect(SPI_DEVICE_PARAM, w5500_socket_t s);
uint8_t w5500_disconnect(SPI_DEVICE_PARAM, w5500_socket_t s);
uint8_t w5500_send_data(SPI_DEVICE_PARAM, w5500_socket_t s);
uint8_t w5500_check_tcp_conn(SPI_DEVICE_PARAM, w5500_socket_t s);

/*Helper Functions*/
uint8_t w5500_split16(uint16_t num, uint8_t * dst);
uint16_t w5500_concat16(uint8_t* num);

/*Error Checking Functions*/
uint8_t w5500_check_status(SPI_DEVICE_PARAM, w5500_socket_t s);
uint8_t w5500_check_interrupt(SPI_DEVICE_PARAM, w5500_socket_t s);

/** @brief Reads or writes len bytes starting at address reg
 *
 * @param SPI_DEVICE_PARAM the spi device to communicate with
 * @param reg the register address to start write/read
 * @param s the socket number to write/read
 * @param data data to write to socket/address to store data at
 * @param len length of bytes being read/written
 * @param write if true; write data; else read data
 * @return 1 to indicate success
 */
uint8_t w5500_rw(SPI_DEVICE_PARAM, uint16_t reg, w5500_socket_t s, void* data, size_t len, bool write);



void w5500_print_reg(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg, uint8_t len);
void w5500_print_all(SPI_DEVICE_PARAM , w5500_socket_t s);
