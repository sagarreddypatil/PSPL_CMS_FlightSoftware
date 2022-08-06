#ifndef ETHERNUT_H
#define ETHERNUT_H

#include "spi.h"

// Common register addresses (Section 3.1)

// Mode Register
#define ETH_MR 0x0000
// Gateway IP Regsisters
#define ETH_GAR 0x0001
#define ETH_GAR_0 0x0001
#define ETH_GAR_1 0x0002
#define ETH_GAR_2 0x0003
#define ETH_GAR_3 0x0004
// Subnet Mask Registers
#define ETH_SUBR 0x0005
#define ETH_SUBR_0 0x0005
#define ETH_SUBR_1 0x0006
#define ETH_SUBR_2 0x0007
#define ETH_SUBR_3 0x0008
// Source Hardware Address Registers
#define ETH_SHAR 0x0009
#define ETH_SHAR_0 0x0009
#define ETH_SHAR_1 0x000A
#define ETH_SHAR_2 0x000B
#define ETH_SHAR_3 0x000C
#define ETH_SHAR_4 0x000D
#define ETH_SHAR_5 0x000E
// Source IP Address Register
#define ETH_SUBR 0x000F
#define ETH_SUBR_0 0x000F
#define ETH_SUBR_1 0x0010
#define ETH_SUBR_2 0x0011
#define ETH_SUBR_3 0x0012
// Interrupt Low Level Timer
#define ETH_INTLEVEL 0x0013
#define ETH_INTLEVEL_0 0x0013
#define ETH_INTLEVEL_1 0x0014
// Interrupt
#define ETH_IR 0x0015
// Interrupt Mask
#define ETH_IMR 0x0016
// Socket Interrupt
#define ETH_SIR 0x0017
// Socket Interrupt Mask
#define ETH_SIMR 0x0018
// Retry Time
#define ETH_RTR 0x0019
#define ETH_RTR_0 0x0019
#define ETH_RTR_1 0x001A
// Retry Counter
#define ETH_RCR 0x001B
// PPP LCP Request Timer
#define ETH_PTIMER 0x001C
// PPP LCP Magic number
#define PMAGIN 0x001D
// PPP Destination MAC Address
#define ETH_PHAR 0x001E
#define ETH_PHAR_0 0x001E
#define ETH_PHAR_1 0x001F
#define ETH_PHAR_2 0x0020
#define ETH_PHAR_3 0x0021
#define ETH_PHAR_4 0x0022
#define ETH_PHAR_5 0x0023
// PPP Session Identification
#define ETH_PSID 0x0024
#define ETH_PSID_0 0x0024
#define ETH_PSID_1 0x0025
// PPP Maximum Segment Size
#define ETH_PMRU 0x0026
#define ETH_PMRU_0 0x0026
#define ETH_PMRU_1 0x0027
// Unreachable IP Address
#define ETH_UIPR 0x0028
#define ETH_UIPR_0 0x0028
#define ETH_UIPR_1 0x0029
#define ETH_UIPR_2 0x002A
#define ETH_UIPR_3 0x002B
// Unreachable Port
#define ETH_UPORTR 0x002C
#define ETH_UPORTR_0 0x002C
#define ETH_UPORTR_1 0x002D
// PHY Configuration
#define ETH_PHYCFGR 0x002E
// Chip Version
#define ETH_VERSIONR 0x0039

// Socket registers (Section 3.2)

// Socket Mode Register
#define ETH_SOCK_MR 0x0000
// Socket Command Register
#define ETH_SOCK_CR 0x0001
// Socket Interrupt
#define ETH_SOCK_IR 0x0002
// Socket Status
#define ETH_SOCK_SR 0x0003
// Socket Source Port
#define ETH_SOCK_PORT 0x0004
#define ETH_SOCK_PORT_0 0x0004
#define ETH_SOCK_PORT_1 0x0005
// Socket Destination Hardware Address
#define ETH_SOCK_DHAR 0x0006
#define ETH_SOCK_DHAR_0 0x0006
#define ETH_SOCK_DHAR_1 0x0007
#define ETH_SOCK_DHAR_2 0x0008
#define ETH_SOCK_DHAR_3 0x0009
#define ETH_SOCK_DHAR_4 0x000A
#define ETH_SOCK_DHAR_5 0x000B
// Socket Destination IP Address
#define ETH_SOCK_DIPR 0x000C
#define ETH_SOCK_DIPR_0 0x000C
#define ETH_SOCK_DIPR_1 0x000D
#define ETH_SOCK_DIPR_2 0x000E
#define ETH_SOCK_DIPR_3 0x000F
// Socket Destination Port
#define ETH_SOCK_DPORT 0x0010
#define ETH_SOCK_DPORT_0 0x0010
#define ETH_SOCK_DPORT_1 0x0011
// Socket Maximum Segment Size
#define ETH_SOCK_MSSR 0x0012
#define ETH_SOCK_MSSR_0 0x0012
#define ETH_SOCK_MSSR_1 0x0013
// Socket IP TOS
#define ETH_SOCK_TOS 0x0015
// Socket IP TTL
#define ETH_SOCK_TTL 0x0016
// Socket Receive Buffer Size
#define ETH_SOCK_RXBUF_SIZE 0x001E
// Socket Transmit Buffer Size
#define ETH_SOCK_TXBUF_SIZE 0x001F
// Socket TX Free Size
#define ETH_SOCK_TX_FSR 0x0020
#define ETH_SOCK_TX_FSR_0 0x0020
#define ETH_SOCK_TX_FSR_1 0x0021
// Socket TX Read Pointer
#define ETH_SOCK_TX_RD 0x0022
#define ETH_SOCK_TX_RD_0 0x0022
#define ETH_SOCK_TX_RD_1 0x0023
// Socket TX Write Pointer
#define ETH_SOCK_TX_WR 0x0024
#define ETH_SOCK_TX_WR_0 0x0024
#define ETH_SOCK_TX_WR_1 0x0025
// Socket RX Reveived Size
#define ETH_SOCK_RX_RSR 0x0026
#define ETH_SOCK_RX_RSR_0 0x0026
#define ETH_SOCK_RX_RSR_1 0x0027
// Socket RX Read Pointer
#define ETH_SOCK_RX_RD 0x0028
#define ETH_SOCK_RX_RD_0 0x0028
#define ETH_SOCK_RX_RD_1 0x0029
// Socket RX Write Pointer
#define ETH_SOCK_RX_WR 0x002A
#define ETH_SOCK_RX_WR_0 0x002A
#define ETH_SOCK_RX_WR_1 0x002B
// Socket Interrupt Mask
#define ETH_SOCK_IMR 0x002C
// Socket Fragment Offset in IP Header
#define ETH_SOCK_FRAG 0x002D
#define ETH_SOCK_FRAG_0 0x002D
#define ETH_SOCK_FRAG_1 0x002E
// Keep alive timer
#define ETH_SOCK_KPALVTR 0x002F

// Control byte blocks (2.2.2)
#define ETH_COMMON_REG 0x00
#define ETH_SOCK_0_REG 0x01
#define ETH_SOCK_0_TX_BUFF 0x02
#define ETH_SOCK_0_RX_BUFF 0x03
// 0x04 Reserved
#define ETH_SOCK_1_REG 0x05
#define ETH_SOCK_1_TX_BUFF 0x06
#define ETH_SOCK_1_RX_BUFF 0x07
// 0x08 Reserved
#define ETH_SOCK_2_REG 0x09
#define ETH_SOCK_2_TX_BUFF 0x0A
#define ETH_SOCK_2_RX_BUFF 0x0B
// 0x0C Reserved
#define ETH_SOCK_3_REG 0x0D
#define ETH_SOCK_3_TX_BUFF 0x0E
#define ETH_SOCK_3_RX_BUFF 0x0F
// 0x10 Reserved
#define ETH_SOCK_4_REG 0x11
#define ETH_SOCK_4_TX_BUFF 0x12
#define ETH_SOCK_4_RX_BUFF 0x13
// 0x14 Reserved
#define ETH_SOCK_5_REG 0x15
#define ETH_SOCK_5_TX_BUFF 0x16
#define ETH_SOCK_5_RX_BUFF 0x17
// 0x18 Reserved
#define ETH_SOCK_6_REG 0x19
#define ETH_SOCK_6_TX_BUFF 0x1A
#define ETH_SOCK_6_RX_BUFF 0x1B
// 0x1C Reserved
#define ETH_SOCK_7_REG 0x1D
#define ETH_SOCK_7_TX_BUFF 0x1E
#define ETH_SOCK_7_RX_BUFF 0x1F

// Read write bit
#define ETH_READ 0
#define ETH_WRITE 1

// SPI Operation mode
#define ETH_VAR_LENGTH 0
#define ETH_1_BYTE_LEN 1
#define ETH_2_BYTE_LEN 2
#define ETH_4_BYTE_LEN 3

// Macro to form the ethernut control byte
#define ETH_CONTROL_BYTE(block, r_w, spi_op_mode) (((block) << 3) | ((r_w) << 2) | (spi_op_mode))

spi_slave_t ethernut_init(volatile uint8_t *, volatile uint8_t *, uint8_t);

#endif