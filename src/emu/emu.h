#include <pico/stdlib.h>

#include <w5500.h>
#include <ads13x.h>
#include <w25n01.h>
#include <max31856.h>

#include <commandnet.h>
#include <ntp.h>

//------------Devices------------
SPI_DEVICE(tc_0, spi0, 1);
SPI_DEVICE(tc_1, spi0, 0);
SPI_DEVICE(adc_0, spi0, 6);
SPI_DEVICE(flash, spi0, 20);
SPI_DEVICE(w5500, spi1, 25);

//------------Global Vars------------
extern uint64_t time_offset;

// CommandNet Arrays

extern cmdnet_cmd_t cmds[];
extern const size_t cmds_len;

extern cmdnet_var_t vars[];
extern const size_t vars_len;

//------------Constants------------
// Network
static const ip_t gateway     = {192, 168, 2, 1};
static const ip_t subnet_mask = {255, 255, 255, 0};
static const ip_t src_ip      = {192, 168, 2, 50};

// GPIO
static const uint LOX_SOLENOID = 16;
static const uint ETH_SOLENOID = 17;

static const uint PYRO = 28;

// ADC Builtin

static const uint PYRO_CONT_0       = 26;
static const uint PYRO_CONT_0_INPUT = 0;
static const uint PYRO_CONT_1       = 27;
static const uint PYRO_CONT_1_INPUT = 1;

//------------Tasks------------
void cmdnet_task_init();
void cmdnet_task_run();

void sensornet_task_init();
void sensornet_task_run();
