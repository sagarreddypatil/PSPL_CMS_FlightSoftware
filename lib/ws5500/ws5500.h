/*
Datasheet  
https://cdn.sparkfun.com/datasheets/Dev/Arduino/Shields/W5500_datasheet_v1.0.2_1.pdf
*/

#pragma once
#include <spi_device_impl.h>

/*===============================
Common Register Addresses pg 32
===============================*/

static const uint8_t ws5500_mr = 0x00; //Mode Register
//Gateway IP Address Registers
static const uint8_t ws5500_gar0 = 0x01;
static const uint8_t ws5500_gar1 = 0x02;
static const uint8_t ws5500_gar2 = 0x03;
static const uint8_t ws5500_gar3 = 0x04;
//Subnet Mask Registers
static const uint8_t ws5500_subr0 = 0x05;
static const uint8_t ws5500_subr1 = 0x06;
static const uint8_t ws5500_subr2 = 0x07;
static const uint8_t ws5500_subr3 = 0x08;
//Source Hardware Address Registers
static const uint8_t ws5500_shar0 = 0x09;
static const uint8_t ws5500_shar1 = 0x0A;
static const uint8_t ws5500_shar2 = 0x0B;
static const uint8_t ws5500_shar3 = 0x0C;
static const uint8_t ws5500_shar4 = 0x0D;
static const uint8_t ws5500_shar5 = 0x0E;
//Source IP Address Registers
static const uint8_t ws5500_sipr0 = 0x0F;
static const uint8_t ws5500_sipr1 = 0x10;
static const uint8_t ws5500_sipr2 = 0x11;
static const uint8_t ws5500_sipr4 = 0x12;
//Interrupt Level Address Registers
static const uint8_t ws5500_intLevel0 = 0x13;
static const uint8_t ws5500_intLevel1 = 0x14;
static const uint8_t ws5500_ir = 0x15; //Interrupt Register
static const uint8_t ws5500_imr = 0x16; //Interrupt Mask Register
static const uint8_t ws5500_sir = 0x17; //Socket Interrupt Register
static const uint8_t ws5500_simr = 0x18; //Socket Interrupt Mask Register
//Retry Time Value Registers
static const uint8_t ws5500_rtr0 = 0x19;
static const uint8_t ws5500_rtr1 = 0x1A;
static const uint8_t ws5500_rcr = 0x1B; //Retry Count Register
static const uint8_t ws5500_ptimer = 0x1C; //PPP Link Control Protocol Request Timer Register
static const uint8_t ws5500_pmagic = 0x1D; //PPP Link Control Protocol Magic Number Register
//Destination Hardware Address Registers (PPPoE Mode)
static const uint8_t ws5500_phar0 = 0x1E;
static const uint8_t ws5500_phar1 = 0x1F;
static const uint8_t ws5500_phar2 = 0x20;
static const uint8_t ws5500_phar3 = 0x21;
static const uint8_t ws5500_phar4 = 0x22;
static const uint8_t ws5500_phar5 = 0x23;
//Session ID Registers (PPPoE Mode)
static const uint8_t ws5500_psid0 = 0x24;
static const uint8_t ws5500_psid1 = 0x25;
//Maximum Recieve Unit Registers (PPPoE Mode)
static const uint8_t ws5500_pmru0 = 0x26;
static const uint8_t ws5500_pmru1 = 0x27;
//Unreachable IP Address Registers
static const uint8_t ws5500_uipr0 = 0x28;
static const uint8_t ws5500_uipr1 = 0x29;
static const uint8_t ws5500_uipr2 = 0x2A;
static const uint8_t ws5500_uipr3 = 0x2B;
//Unreachable Port Registers
static const uint8_t ws5500_uportr0 = 0x2C;
static const uint8_t ws5500_uportr1 = 0x2D;
static const uint8_t ws5500_phycfgr = 0x2E; //PHY Configuration Register
static const uint8_t ws5500_versionr = 0x39; //Chip Version Register

/*===============================
Socket Register Addresses pg 44
===============================*/

static const uint8_t socket_mr = 0x00; //Socket Mode Register
static const uint8_t socket_cr = 0x01; //Socket Command Register
static const uint8_t socket_ir = 0x02; //Socket Interrupt Register
static const uint8_t socket_status = 0x03; //Socket Status Register
//Socket Source Port Registers
static const uint8_t socket_sport0 = 0x04;
static const uint8_t socket_sport1 = 0x05;
//Socket Destination Hardware Address Registers 
static const uint8_t socket_dhar0 = 0x06;
static const uint8_t socket_dhar1 = 0x07;
static const uint8_t socket_dhar2 = 0x08;
static const uint8_t socket_dhar3 = 0x09;
static const uint8_t socket_dhar4 = 0x0A;
static const uint8_t socket_dhar5 = 0x0B;
//Socket Destination IP Address Registers
static const uint8_t socket_dipr0 = 0x0C;
static const uint8_t socket_dipr1 = 0x0D;
static const uint8_t socket_dipr2 = 0x0E;
static const uint8_t socket_dipr3 = 0x0F;
//Socket Destination Port Registers
static const uint8_t socket_dport0 = 0x10;
static const uint8_t socket_dport1 = 0x11;
//Socket Maximum Segment Size Registers
static const uint8_t socket_mssr0 = 0x12;
static const uint8_t socket_mssr1 = 0x13;
static const uint8_t socket_tos = 0x15; //Type of Service Register
static const uint8_t socket_ttl = 0x16; //Time to Live Configuration Register
static const uint8_t socket_rbuf_size = 0x1E; //Socket Rx Buffer Size Register
static const uint8_t socket_tbuf_size = 0x1F; //Socket Tx Buffer Size Register
//Socket Tx Free Size Registers 
static const uint8_t socket_tx_fsr0 = 0x20; 
static const uint8_t socket_tx_fsr1 = 0x21;
//Socket Tx Read Pointer Registers
static const uint8_t socket_rd0 = 0x22;
static const uint8_t socket_rd1 = 0x23;
//Socket Tx Write Pointer Registers
static const uint8_t socket_wr0 = 0x24;
static const uint8_t socket_wr1 = 0x25;
//Socket Recieved Size Register
static const uint8_t socket_rsr0 = 0x26;
static const uint8_t socket_rsr1 = 0x27;
//Socket Rx Read Data Pointer Register
static const uint8_t socket_rd0 = 0x28;
static const uint8_t socket_rd1 = 0x29;
//Socket Rx Write Pointer Register
static const uint8_t socket_wr0 = 0x2A;
static const uint8_t socket_wr1 = 0x2B;
static const uint8_t socket_imr = 0x23; //Socket Interrupt Mask Register
//Socket Fragment Registers
static const uint8_t socket_frag0 = 0x2D;
static const uint8_t socket_frag1 = 0x2E;
static const uint8_t socket_pmru0 = 0x2F; //Socket Keep Alive Time Register


/*Functionality: 
read & write data to single register (socket & common)
config ethernet switch for UDP multicasting
config rx tx buffer size
send ethernet data to relevant chips
recieve spi data from relevant chips and transfer over ethernet
configure socket port number, MAC Address, IP Address


*/
