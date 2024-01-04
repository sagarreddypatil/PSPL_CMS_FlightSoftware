#include <pico/stdlib.h>
#include <w5500.h>
#include <ads13x.h>
#include <commandnet.h>
#include <sensornet.h>
#include <ntp.h>

//------------Devices------------
extern myspi_device_t w5500;
extern myspi_device_t ads13x;

//------------Task Handles------------
extern TaskHandle_t w5500_drdy_task;

//------------Global Vars------------
extern int64_t lox_period;
extern int64_t lox_duty_cycle;

extern int64_t eth_period;
extern int64_t eth_duty_cycle;

extern int64_t lox_run;
extern int64_t eth_run;

extern int64_t lox_state;
extern int64_t eth_state;

extern uint64_t time_offset;
extern int64_t pyro_state;

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

void solenoid_task_init();
void solenoid_task_run();

void w5500_drdy_handler();