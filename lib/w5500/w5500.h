/** @file w5500.h
 *  @brief function prototypes for the w5500 driver
 *
 * @author Jay S. Jagani (jjagani)
 *
 */

#pragma once
#include <spi_device.h>

/*
 * Common Register Addresses pg 32
 */

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

/*
 * Socket Register Addresses pg 44
 */

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

/*
 * Socket numbers
 */

typedef enum {
  cmn = 0b00000,
  s0  = 0b00001,
  s1  = 0b00101,
  s2  = 0b01001,
  s3  = 0b01101,
  s4  = 0b10001,
  s5  = 0b10101,
  s6  = 0b11001,
  s7  = 0b11101,
} w5500_socket_t;

/*
 * Protocols for sockets
 */

typedef enum {
  socket_closed = 0x00,
  tcp           = 0x01,
  udp           = 0x02,
} w5500_protocol_t;

/*
 * Socket commands
 */

typedef enum {
  w5500_cr_open       = 0x01,
  w5500_cr_listen     = 0x02,
  w5500_cr_connect    = 0x04,
  w5500_cr_disconnect = 0x08,
  w5500_cr_close      = 0x10,
  w5500_cr_send       = 0x20,
  w5500_cr_send_mac   = 0x21,
  w5500_cr_send_keep  = 0x22,
  w5500_cr_recv       = 0x40,
} w5500_cr_t;

/*
 * Interrupt values that w5500 can return (error checking during socket operation)
 */

typedef enum {
  w5500_ir_SENDOK  = 0x10,
  w5500_ir_TIMEOUT = 0x08,
  w5500_ir_RECV    = 0x04,
  w5500_ir_DISCON  = 0x02,
  w5500_ir_CON     = 0x00,
} w5500_ir_t;

/*
 * Status Codes that w5500 can return (error checking in socket initialization)
 */

typedef enum {
  w5500_SOCK_CLOSED      = 0x00,
  w5500_SOCK_INIT        = 0x13,
  w5500_SOCK_LISTEN      = 0x14,
  w5500_SOCK_ESTABLISHED = 0x17,
  w5500_SOCK_CLOSE_WAIT  = 0x1C,
  w5500_SOCK_UDP         = 0x22,
  w5500_SOCK_SYNSENT     = 0x15,
  w5500_SOCK_SYNRECV     = 0x16,
  w5500_SOCK_FIN_WAIT    = 0x18,
  w5500_SOCK_CLOSING     = 0x1A,
  w5500_SOCK_TIME_WAIT   = 0x1B,
  w5500_SOCK_LAST_ACK    = 0x1D
} w5500_sr_t;

typedef uint8_t ip_t[4];
typedef uint8_t mac_t[6];

SPI_INITFUNC(w5500);

/** @brief Reads or writes len bytes starting at address reg
 *
 * @param SPI_DEVICE_PARAM the spi device to communicate with
 * @param reg the register address to start write/read
 * @param s the socket number to write/read
 * @param data data to write to socket/address to store data at
 * @param len length of bytes being read/written
 * @param write if true, write data, else read data
 * @return 1 to indicate success
 */
uint8_t __w5500_rw(SPI_DEVICE_PARAM, uint16_t reg, w5500_socket_t s, void* data, size_t len, bool write);

/** @brief Initializes w5500
 *
 * @param gateway: gateway address to connect to Ex: for 192.168.2.100, gateway = {192, 168, 2, 100}
 * @param sub_mask: subnet mask  Ex: for 255.255.255.0,  sub_mask = {255,255,255,0}
 * @param src_ip: ip address of w5500 Ex: see gateway
 * @param mac_addr: mac address of w5500 Ex: for 1.2.3.4.5.6, mac_addr = {1,2,3,4,5,6}
 * @param wol: if true - wake on LAN
 * @param ping_block: if true - w5500 will block ping requests (in general, leave false)
 * @param farp: if true - w5500 will send an ARP request every time data is sent (unless debugging, leave false)
 * @return 1 to confirm success
 */
uint8_t w5500_init(SPI_DEVICE_PARAM, ip_t gateway, ip_t sub_mask, ip_t src_ip, mac_t mac_addr, bool wol, bool ping_block, bool farp);

/** @brief Initializes new socket with parameters needed for operation
 * @param s: socket number (s0 - s7)
 * @param protocol: protocol type to use (udp or tcp)
 * @param src_port: port of socket
 * @param dst, dst_port: destination ip address and port of socket
 * @param txbuf_size, rxbuf_size: portion of rx and tx buffer memory to allocate for socket s (2,4,8,16 kB valid values - will implement error checking here)
 * @param multicast: if true - sets up udp socket to multicast (note: in this more socket does not support ARP)
 * @param unicast_block: if true - blocks unicast messages (further testing needed as to whether this blocks ARP requests)
 * @param broadcast_block: if true - blocks all broadcast messages
 * @param dhar: ONLY needed when using multicast UDP. All devices recieving packets on the multicast group must have dhar as their mac address. (see mac_addr in w5500_init for initialization)
 * @return 1 to confirm success
 */
uint8_t w5500_socket_init(SPI_DEVICE_PARAM, w5500_socket_t s, w5500_protocol_t protocol, uint16_t src_port, ip_t dst, uint16_t dst_port, uint8_t txbuf_size, uint8_t rxbuf_size, bool multicast, bool unicast_block, bool broadcast_block, mac_t dhar);

/** @brief Writes len bytes from data into transmit buffer for socket s. This is data ready to send to the network
 * @param SPI_DEVICE_PARAM spi device to communicate with
 * @param s the socket number to write/read
 * @param data data to write to socket/address to store data at
 * @param len length of bytes being read/written
 * @return 1 to confirm success
 */
uint8_t w5500_write_tx(SPI_DEVICE_PARAM, w5500_socket_t s, void* data, size_t len);

/** @brief Processes all data in recieve buffer and puts it into recv_buf
 * @param SPI_DEVICE_PARAM spi device to communicate with
 * @param s the socket number to write/read
 * @param recv_buf the location to start writing recieved data to
 * @return the length of recieved data
 */
uint16_t w5500_recv(SPI_DEVICE_PARAM, w5500_socket_t s, void* recv_buf);

void w5500_print_reg(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg, uint8_t len);
void w5500_print_all(SPI_DEVICE_PARAM, w5500_socket_t s);

/*
 * Automatically creates functions for sending commands to w5500
 */

#define __w5500_command(command)                                                  \
  static inline uint8_t w5500_cmd_##command(SPI_DEVICE_PARAM, w5500_socket_t s) { \
    w5500_cr_t cmd = w5500_cr_##command;                                          \
    __w5500_rw(spi, w5500_socket_cr, s, &cmd, 1, true);                           \
    return 1;                                                                     \
  }

__w5500_command(open);
__w5500_command(close);
__w5500_command(listen);
__w5500_command(connect);
__w5500_command(disconnect);
__w5500_command(send_mac);
__w5500_command(send_keep);
__w5500_command(send);
__w5500_command(recv);

#undef w5500_command