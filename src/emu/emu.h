#include <pico/stdlib.h>
#include <w5500.h>
#include <commandnet.h>

//------------Devices------------
SPI_DEVICE(w5500, spi0, 17)

//------------Global Vars------------
extern int64_t lox_period;
extern int64_t lox_duty_cycle;
extern int64_t eth_period;
extern int64_t eth_duty_cycle;
extern int64_t lox_run;
extern int64_t eth_run;

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
static const uint LOX_SOLENOID = 26;
static const uint ETH_SOLENOID = 27;

//------------Tasks------------
void cmdnet_task_init();
void cmdnet_task_run();

void solenoid_task_init();
void solenoid_task_run();