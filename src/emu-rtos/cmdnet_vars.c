#include "emu.h"
#include <commandnet.h>

uint64_t time_offset = 0;

int64_t lox_period     = 140000;
int64_t lox_duty_cycle = 50000;

int64_t eth_period     = 140000;
int64_t eth_duty_cycle = 50000;

int64_t lox_run = 1;
int64_t eth_run = 1;

int64_t lox_state = 0;
int64_t eth_state = 0;

int64_t pyro_state = 0;

cmdnet_cmd_t cmds[]   = {};
const size_t cmds_len = sizeof(cmds) / sizeof(cmds[0]);

cmdnet_var_t vars[] = {
    // {"lox_period", &lox_period}, {"lox_duty_cycle", &lox_duty_cycle},
    // {"eth_period", &eth_period}, {"eth_duty_cycle", &eth_duty_cycle},
    // {"lox_run", &lox_run},       {"eth_run", &eth_run},
    // {"lox_state", &lox_state},   {"eth_state", &eth_state},
    {"pyro_state", &pyro_state},
    {"time_offset_us", (int64_t*)&time_offset}
};
const size_t vars_len = sizeof(vars) / sizeof(vars[0]);
