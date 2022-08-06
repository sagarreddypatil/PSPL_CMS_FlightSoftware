// Using the ADS130B04
// Datasheet: https://www.ti.com/lit/ds/symlink/ads130b04-q1.pdf?HQS=dis-mous-null-mousermode-dsf-pf-null-wwe&ts=1659723067109&ref_url=https%253A%252F%252Fwww.mouser.com%252F

#ifndef ADC_H
#define ADC_H

#include <stdlib.h>
#include <stdbool.h>
#include "spi.h"

// 4 analog channels on the ADC
#define ADC_CHANNEL_COUNT 4

// 12 bytes and 6 words per SPI frame (assuming 16 bit word size)
#define ADC_FRAME_SIZE 12
#define ADC_WORDS_PER_FRAME 6
#define ADC_WORD_SIZE 2

// ADS130 Registers
#define ADC_ID_REGISTER 0x00
#define ADC_MODE_REGISTER 0x02
#define ADC_CLOCK_REGISTER 0x03
#define ADC_GAIN_REGISTER 0x04
#define ADC_CH0_CFG_REGISTER 0x09
#define ADC_CH1_CFG_REGISTER 0x0E
#define ADC_CH2_CFG_REGISTER 0x13
#define ADC_CH3_CFG_REGISTER 0x18

// ADS130 commands (16 bit)
#define ADC_NULL_COMMAND 0x0000
#define ADC_RESET_COMMAND 0x0011
#define ADC_STANDBY_COMMAND 0x0022
#define ADC_WAKEUP_COMMAND 0x0033
#define ADC_LOCK_COMMAND 0x0555
#define ADC_UNLOCK_COMMAND 0x0655
#define ADC_RREG_COMMAND 0xA000 
#define ADC_WREG_COMMAND 0x6000 
#define ADC_MODE_COMMAND 0x0010 
#define ADC_CLOCK_COMMAND 0x0F0E 
#define ADC_GAIN_COMMAND 0x0000 

// Format a read register command
#define FORMAT_RREG(address) (ADC_RREG_COMMAND + ((uint16_t)(address << 7)))
// Format a write register command
#define FORMAT_WREG(address) (ADC_WREG_COMMAND + ((uint16_t)(address << 7)))

spi_slave_t adc_init(volatile uint8_t *, volatile uint8_t *, uint8_t);
void adc_data(spi_slave_t, uint16_t *);
void adc_write_register(spi_slave_t, uint8_t, uint16_t);
void adc_print_frame(spi_slave_t, uint16_t *, uint16_t *);

#endif