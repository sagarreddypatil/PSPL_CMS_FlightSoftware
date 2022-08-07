#include "ethernut.h"

spi_slave_t ethernut_init(volatile uint8_t *data_direction, volatile uint8_t *port, uint8_t slave_select, uint32_t ipv4) {
    spi_slave_t eth = spi_slave_init(data_direction, port, slave_select);
    
    uint8_t wr_command = 0x00;
    uint8_t sock_wr_command = 0x02;
    uint8_t sock_cr_command = 0x01;
    uint8_t sock_rx_buffer = 0x10;
    uint8_t sock_tx_buffer = 0x10;
    uint16_t sock_port = ETH_PORT;

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
    // Set the source port to use
    ethernut_frame(eth, ETH_SOCK_PORT, ETH_SOCK_0_REG, ETH_WRITE, &sock_port, sizeof(sock_port));
    // Set the destination port to use
    ethernut_frame(eth, ETH_SOCK_DPORT, ETH_SOCK_0_REG, ETH_WRITE, &sock_port, sizeof(sock_port));
    // Open the socket
    ethernut_frame(eth, ETH_SOCK_CR, ETH_SOCK_0_REG, ETH_WRITE, &sock_cr_command, sizeof(sock_cr_command));

    return eth;
}

bool ethernut_isdata(spi_slave_t eth) {
    uint8_t sock_status;
    ethernut_frame(eth, ETH_SOCK_IR, ETH_SOCK_0_REG, ETH_READ, &sock_status, sizeof(sock_status));
    return (bool)((sock_status & ETH_RECV) >> ETH_RECV);
}

bool ethernut_issent(spi_slave_t eth) {
    uint8_t sock_status;
    ethernut_frame(eth, ETH_SOCK_IR, ETH_SOCK_0_REG, ETH_READ, &sock_status, sizeof(sock_status));
    return (bool)((sock_status & ETH_SEND_OK) >> ETH_SEND_OK);
}

void ethernut_recieve(spi_slave_t eth, void *data, uint8_t len) {
    uint16_t eth_rx_pointer;

    // Get the point to read from in the RX Buffer
    ethernut_frame(eth, ETH_SOCK_RX_RD, ETH_SOCK_0_REG, ETH_READ, &eth_rx_pointer, sizeof(eth_rx_pointer));
    // Read the data from the RX buffer
    ethernut_frame(eth, eth_rx_pointer, ETH_SOCK_0_RX_BUFF, ETH_READ, data, len);
    // Add the length of the data that we just read from the RX buffer
    eth_rx_pointer += len;
    // Update the RX Pointer
    ethernut_frame(eth, ETH_SOCK_RX_RD, ETH_SOCK_0_REG, ETH_WRITE, &eth_rx_pointer, sizeof(eth_rx_pointer));
}

void ethernut_transmit(spi_slave_t eth, void *data, uint8_t len, uint32_t ipv4) {
    uint16_t eth_tx_pointer;
    uint8_t eth_send_command = 0x20;

    spi_select(eth);
    
    // Set the destination IP
    ethernut_frame(eth, ETH_SOCK_DIPR, ETH_SOCK_0_REG, ETH_WRITE, &ipv4, sizeof(ipv4));
    // Get the point to write to the TX Buffer
    ethernut_frame(eth, ETH_SOCK_TX_WR, ETH_SOCK_0_REG, ETH_READ, &eth_tx_pointer, sizeof(eth_tx_pointer));
    // Write the data to the TX buffer
    ethernut_frame(eth, eth_tx_pointer, ETH_SOCK_0_TX_BUFF, ETH_WRITE, data, len);
    // Add the length of the data that we just sent to the TX pointer
    eth_tx_pointer += len;
    // Update the TX Pointer
    ethernut_frame(eth, ETH_SOCK_TX_WR, ETH_SOCK_0_TX_BUFF, ETH_WRITE, &eth_tx_pointer, sizeof(eth_tx_pointer));
    // Send the data
    ethernut_frame(eth, ETH_SOCK_CR, ETH_SOCK_0_REG, ETH_WRITE, &eth_send_command, sizeof(eth_send_command));

    spi_deselect(eth);
}

/*
Ethernet SPI Frame

Args:
    spi_slave_t eth: Ethernet slave device
    uint8_t eth_register: 16 bit ethernet register
    uint8_t block_select: What block we want to write to
    uint8_t r_w: Read or Write?
    void *data: Data array that we want to read or write
    uint8_t len: length of the data array
*/
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