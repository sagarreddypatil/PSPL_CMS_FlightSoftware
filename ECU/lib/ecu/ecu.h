#ifndef ECU_H
#define ECU_H

#include "adc.h"
#include "flash.h"
#include "ethernut.h"

// Ox PID Gains
#define P_OX 1
#define I_OX 0
#define D_OX 0

// Fuel PID Gains
#define P_FUEL 1
#define I_FUEL 0
#define D_FUEL 0

// Constants for heath checks
#define ECU_BOUND 10
#define ECU_NUM_FAILURES 10

// Device constants
#define ECU_ADC_0_SS PINB0
#define ECU_ETH_0_SS PINB1
#define ECU_FLASH_0_SS PINB2

// ECU Network Constants
#define ECU_IP 0xBCBCBC01

// Data Structure
typedef struct {
    uint16_t pressure_1;
    uint16_t pressure_2;
    uint16_t pressure_3;
} ecu_data_t;

typedef struct {
    uint8_t pressure_1: 1;
    uint8_t pressure_2: 1;
    uint8_t pressure_3: 1;
    uint8_t pressure_1_count;
    uint8_t pressure_2_count;
    uint8_t pressure_3_count;
} ecu_health_t;

typedef struct {
    spi_slave_t adc;
    spi_slave_t eth;
    flash_t flash; 
    ecu_data_t data;
    ecu_health_t health;
} ecu_t;

ecu_t ecu_init();
void ecu_health(ecu_t *);

#endif