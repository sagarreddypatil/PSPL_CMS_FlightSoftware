#include "emu.h"
#include <commandnet.h>

int64_t lox_period     = 140000;
int64_t lox_duty_cycle = 50000;
int64_t eth_period     = 140000;
int64_t eth_duty_cycle = 50000;
int64_t lox_run        = 1;
int64_t eth_run        = 1;

cmdnet_status_t handle_lox_on() {
  gpio_put(LOX_SOLENOID, 1);
  return CMD_SUCCESS;
}

cmdnet_status_t handle_lox_off() {
  gpio_put(LOX_SOLENOID, 0);
  return CMD_SUCCESS;
}

cmdnet_status_t handle_eth_on() {
  gpio_put(ETH_SOLENOID, 1);
  return CMD_SUCCESS;
}

cmdnet_status_t handle_eth_off() {
  gpio_put(ETH_SOLENOID, 0);
  return CMD_SUCCESS;
}

cmdnet_cmd_t cmds[] = {
    {"lox_on", handle_lox_on},
    {"lox_off", handle_lox_off},
    {"eth_on", handle_eth_on},
    {"eth_off", handle_eth_off},
};
const size_t cmds_len = sizeof(cmds) / sizeof(cmds[0]);

cmdnet_var_t vars[] = {
    {"lox_period", &lox_period}, {"lox_duty_cycle", &lox_duty_cycle},
    {"eth_period", &eth_period}, {"eth_duty_cycle", &eth_duty_cycle},
    {"lox_run", &lox_run},       {"eth_run", &eth_run},
};
const size_t vars_len = sizeof(vars) / sizeof(vars[0]);