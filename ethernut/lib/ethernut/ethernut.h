#ifndef ETHERNUT_H
#define ETHERNUT_H

#include "spi.h"

// Register addresses
#define ETH_MODE_REGISTER 0x0000
// Gateway IP Regsisters
#define ETH_GAR_0 0x0001
#define ETH_GAR_1 0x0002
#define ETH_GAR_2 0x0003
#define ETH_GAR_3 0x0004
// Subnet Mask Registers
#define ETH_SUBR_0 0x0005
#define ETH_SUBR_0 0x0006
#define ETH_SUBR_0 0x0007
#define ETH_SUBR_1 0x0008

// Control byte blocks
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
#define CONTROL_BYTE(block, r_w, spi_op_mode) (((block) << 3) | ((r_w) << 2) | (spi_op_mode))
spi_slave_t ethernut_init(volatile uint8_t *, volatile uint8_t *, uint8_t);

#endif