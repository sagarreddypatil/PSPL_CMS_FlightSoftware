/** @file w5500.h
 *  @brief Structures, enums, and functions for the W5500 driver
 *
 * @author Sagar R. Patil (sagarreddypatil), Jay S. Jagani (jjagani)
 *
 */

#pragma once
#include <spi_device.h>

/**
 * @defgroup w5500 W5500 Driver
 * @{
 */

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

/**
 * @brief W5500 Socket Interrupt Register bitfields
 */
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

/**
 * @brief W5500 Commands
 */
typedef enum {
  W5500_CMD_OPEN      = 0x01,  ///< Open a socket
  W5500_CMD_LISTEN    = 0x02,  ///< Listen for a connection
  W5500_CMD_CONNECT   = 0x04,  ///< Connect to a remote host
  W5500_CMD_DISCON    = 0x08,  ///< Disconnect socket
  W5500_CMD_CLOSE     = 0x10,  ///< Close socket
  W5500_CMD_SEND      = 0x20,  ///< Send data on TX buffer
  W5500_CMD_SEND_MAC  = 0x21,  ///< Send MACRAW
  W5500_CMD_SEND_KEEP = 0x22,  ///< Send keep alive
  W5500_CMD_RECV      = 0x40,  ///< Inform RX buffer read
} w5500_socket_command_t;

typedef enum {
  W5500_SOCK_CLOSED      = 0x00,  ///< Socket is closed
  W5500_SOCK_INIT        = 0x13,  ///< TCP Socket is initialized, command LISTEN to start accepting connections
  W5500_SOCK_LISTEN      = 0x14,  ///< TCP Socket is listening for connections
  W5500_SOCK_ESTABLISHED = 0x17,  ///< TCP Socket is connected to client
  W5500_SOCK_CLOSE_WAIT  = 0x1C,  ///< TCP Socket is waiting for a connection termination
  W5500_SOCK_UDP         = 0x22,  ///< UDP Socket is initialized
  W5500_SOCK_MACRAW      = 0x42,  ///< MACRAW Socket is initialized
  W5500_SOCK_SYNSENT     = 0x15,  ///< TCP Socket is sending a connection request
  W5500_SOCK_SYNRECV     = 0x16,  ///< TCP Socket is receiving a connection request, moving to ESTABLISHED
  W5500_SOCK_FIN_WAIT    = 0x18,  ///< no idea
  W5500_SOCK_CLOSING     = 0x1A,  ///< TCP Socket is closing, transient
  W5500_SOCK_TIME_WAIT   = 0x1B,  ///< I have no idea lmao
  W5500_SOCK_LAST_ACK    = 0x1D,  ///< also no idea
} w5500_socket_status_t;

/**
 * @brief W5500 Socket Registers, value is the block select bits
 */
typedef enum {
  W5500_COMMON = 0b00000,  ///< Common register space
  W5500_S0     = 0b00001,
  W5500_S1     = 0b00101,
  W5500_S2     = 0b01001,
  W5500_S3     = 0b01101,
  W5500_S4     = 0b10001,
  W5500_S5     = 0b10101,
  W5500_S6     = 0b11001,
  W5500_S7     = 0b11101,
} w5500_socket_t;

/**
 * @brief Error codes
 */
typedef enum {
  ERR_INSUFFICIENT_MEM = -1,
  SUCCESS              = 0,
} w5500_error_t;

/* Protocols for sockets */

/**
 * @brief W5500 Socket Protocol
 */
typedef enum {
  W5500_PROTOCOL_CLOSED = 0,     ///< Socket is closed
  W5500_PROTOCOL_TCP    = 0b1,   ///< TCP Socket
  W5500_PROTOCOL_UDP    = 0b10,  ///< UDP Socket
  W5500_PROTOCOL_MACRAW = 0b100  ///< MACRAW Socket, for sending raw ethernet frames
} w5500_protocol_t;

/**
 * @brief IP address type
 */
typedef uint8_t ip_t[4];

/**
 * @brief MAC address type
 */
typedef uint8_t mac_t[6];

SPI_INITFUNC(w5500);

/**
 * @brief Read or write memory on the W5500
 * @param spi Pointer to the SPI device
 * @param s Socket number, use W5500_COMMON for common registers
 * @param reg Register address
 * @param write True if writing, false if reading
 * @param data Pointer to the data buffer
 * @param len Length of the data buffer
 */
void w5500_rw(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg, bool write, void* data, size_t len);

/**
 * @brief Read a 1-byte register and return it
 * @param spi Pointer to the SPI device
 * @param s Socket number, use W5500_COMMON for common registers
 * @param reg Register address
 * @return The value stored in the register
 */
static inline uint8_t w5500_read8(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg) {
  uint8_t data;
  w5500_rw(spi, s, reg, false, &data, 1);
  return data;
}

/**
 * @brief Write a 1-byte register
 * @param spi Pointer to the SPI device
 * @param s Socket number, use W5500_COMMON for common registers
 * @param reg Register address
 * @param data The value to write
 */
static inline void w5500_write8(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg, uint8_t data) {
  w5500_rw(spi, s, reg, true, &data, 1);
}

/**
 * @brief Read a 2-byte register and return it
 * @param spi Pointer to the SPI device
 * @param s Socket number, use W5500_COMMON for common registers
 * @param reg Register address
 * @return The value stored in the register
 */
static inline uint16_t w5500_read16(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg) {
  uint8_t buf[2];
  w5500_rw(spi, s, reg, false, buf, 2);
  return ((uint16_t)(buf[0]) << 8) | buf[1];
}

/**
 * @brief Write a 2-byte register
 * @param spi Pointer to the SPI device
 * @param s Socket number, use W5500_COMMON for common registers
 * @param reg Register address
 * @param data The value to write
 */
static inline void w5500_write16(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg, uint16_t data) {
  uint8_t buf[2] = {data >> 8, data & 0xFF};
  w5500_rw(spi, s, reg, true, buf, 2);
}

/**
 * @brief Set a bitfield flag in a register
 * @param spi Pointer to the SPI device
 * @param s Socket number, use W5500_COMMON for common registers
 * @param reg Register address
 * @param bit Bit index to set
 * @param value Value to set the bit to
 */
static inline void w5500_set_flag(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t reg, uint8_t bit, bool value) {
  w5500_write8(spi, s, reg, w5500_read8(spi, s, reg) | ((value & 1) << bit));
}

/*W5500 Constructors*/
/**
 * @brief Reset the W5500
 * @param spi Pointer to the SPI device
 */
void w5500_init(SPI_DEVICE_PARAM);

/**
 * @brief Check if the W5500 is powered on and ready, poll this function until it returns true
 * @param spi Pointer to the SPI device
 * @return true if ready, false if not
 */
bool w5500_ready(SPI_DEVICE_PARAM);

/**
 * @brief Configure the common settings of the W5500
 * @param spi Pointer to the SPI device
 * @param src_mac Deive MAC address
 * @param src_ip Device IP address
 * @param subnet_mask Device subnet mask
 * @param gateway Network gateway address
 */
void w5500_config(SPI_DEVICE_PARAM, mac_t src_mac, ip_t src_ip, ip_t subnet_mask, ip_t gateway);

/**
 * @brief Set WOL mode
 * @param spi Pointer to the SPI device
 * @param wol True to enable WOL, false to disable
 */
void w5500_set_wol(SPI_DEVICE_PARAM, bool wol);

/**
 * @brief Set the ping block mode
 * @param spi Pointer to the SPI device
 * @param block_ping True to block ping, false to allow ping
 */
void w5500_set_ping_block(SPI_DEVICE_PARAM, bool block_ping);

/**
 * @brief Set the force ARP mode, forces ARP for all transmissions
 * @param spi Pointer to the SPI device
 * @param force_arp True to force ARP, false to not force ARP
 */
void w5500_set_force_arp(SPI_DEVICE_PARAM, bool force_arp);

/*Socket Constructors*/
/**
 * @brief Create a UDP socket
 * @param spi Pointer to the SPI device
 * @param s Socket number
 * @param src_port Source port
 * @param multicast True to enable multicast, false to disable
 * @param block_broadcast True to block broadcast, false to allow broadcast
 * @param block_unicast True to block unicast, false to allow unicast
 * @return Error code if one is encountered, SUCCESS otherwise
 */
w5500_error_t w5500_create_udp_socket(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t src_port, bool multicast, bool block_broadcast, bool block_unicast);

/**
 * @brief Create a TCP socket
 * @param spi Pointer to the SPI device
 * @param s Socket number
 * @param src_port Source port
 * @return Error code if one is encountered, SUCCESS otherwise
 */
w5500_error_t w5500_create_tcp_socket(SPI_DEVICE_PARAM, w5500_socket_t s, uint16_t src_port);

/*Writing and Reading for Data Transfer*/

/**
 * @brief Read data from a socket
 * @param spi Pointer to the SPI device
 * @param s Socket number
 * @param data Pointer to the buffer to store the data in
 * @param len Length of the data to read
 */
void w5500_read_data(SPI_DEVICE_PARAM, w5500_socket_t s, uint8_t* data, size_t len);

/**
 * @brief Write data to a socket
 * @param spi Pointer to the SPI device
 * @param s Socket number
 * @param data Pointer to the data to write
 * @param len Length of the data to write
 * @return ERR_INSUFFICIENT_MEM if the socket buffer is full, SUCCESS otherwise
 */
w5500_error_t w5500_write_data(SPI_DEVICE_PARAM, w5500_socket_t s, void* data, size_t len);  // keep alive is for TCP only

/* Sends commands to w5500*/
/**
 * @brief Send a command to a socket
 * @param spi Pointer to the SPI device
 * @param s Socket number
 * @param command Command to send
 */
void w5500_command(SPI_DEVICE_PARAM, w5500_socket_t s, w5500_socket_command_t command);

/**
 * @}
 */