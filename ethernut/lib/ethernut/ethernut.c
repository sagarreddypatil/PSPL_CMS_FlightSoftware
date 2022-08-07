#include "ethernut.h"

spi_slave_t ethernut_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select, uint32_t ipv4) {
    spi_slave_t eth = spi_slave_init(data_direction, port, slave_select);
    
    uint8_t wr_command = 0x00;
    uint8_t sock_wr_command = 0x02;
    uint8_t sock_cr_command = 0x01;
    uint8_t sock_rx_buffer = 0x10;
    uint8_t sock_tx_buffer = 0x10;
    uint16_t sock_port = 0xBCBC;

    // Configure the common registers
    // Set the Mode Register
    ethernut_frame(eth, ETH_MR, ETH_COMMON_REG, ETH_WRITE, &wr_command, sizeof(wr_command));
    // Set the IP Address
    ethernut_frame(eth, ETH_SIPR, ETH_COMMON_REG, ETH_WRITE, &ipv4, sizeof(ipv4));

    // Configure the socket registers
    // Set the socket to use UDP
    ethernut_frame(eth, ETH_SOCK_MR, ETH_SOCK_0_REG, ETH_WRITE, &sock_wr_command, sizeof(sock_wr_command));
    // Set the socket RX Buffer size to 16KB
    ethernut_frame(eth, ETH_SOCK_RXBUF_SIZE, ETH_SOCK_0_REG, ETH_WRITE, &sock_rx_buffer, sizeof(sock_rx_buffer));
    // Set the socket TX Buffer size to 16KB
    ethernut_frame(eth, ETH_SOCK_TXBUF_SIZE, ETH_SOCK_0_REG, ETH_WRITE, &sock_tx_buffer, sizeof(sock_tx_buffer));
    // Set the port to use
    ethernut_frame(eth, ETH_SOCK_PORT, ETH_SOCK_0_REG, ETH_WRITE, &sock_port, sizeof(sock_port));
    // Set the socket to open
    ethernut_frame(eth, ETH_SOCK_CR, ETH_SOCK_0_REG, ETH_WRITE, &sock_cr_command, sizeof(sock_cr_command));

    return eth;
}

bool ethernut_isdata(spi_slave_t eth) {
    uint8_t is_data;

    ethernut_frame(eth, ETH_SOCK_IR, ETH_SOCK_0_REG, ETH_READ, &is_data, sizeof(is_data));

    if(is_data & ETH_RECV) {
        return true;
    }
    return false;
}

void ethernut_recieve(spi_slave_t eth, uint8_t *data, uint8_t len) {
    uint16_t eth_read_pointer;

    ethernut_frame(eth, ETH_SOCK_RX_RD, ETH_SOCK_0_REG, ETH_READ, &eth_read_pointer, sizeof(eth_read_pointer));
    ethernut_frame(eth, eth_read_pointer, ETH_SOCK_0_RX_BUFF, ETH_READ, data, len);
    eth_read_pointer += len;
    ethernut_frame(eth, ETH_SOCK_RX_RD, ETH_SOCK_0_REG, ETH_WRITE, &eth_read_pointer, sizeof(eth_read_pointer));
}

void ethernut_frame(spi_slave_t eth, uint16_t eth_register, uint8_t block_select, uint8_t r_w, void *data, uint8_t len) {
    uint8_t control_byte = ETH_CONTROL_BYTE(block_select, r_w, ETH_VAR_LENGTH);
    eth_register = ETH_REVERSE(eth_register);

    spi_select(eth);
    
    // Initialize the ethernut SPI frame
    spi_transaction(&eth_register, NULL, sizeof(eth_register));
    spi_transaction(&control_byte, NULL, sizeof(control_byte));

    // Transmit/Recieve the number of requested bytes
    if(r_w == ETH_WRITE) {
        spi_transaction(data, NULL, len);
    } else {
        spi_transaction(NULL, data, len);
    }

    spi_deselect(eth);
}