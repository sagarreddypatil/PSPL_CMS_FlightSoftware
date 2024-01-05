#include "emu.h"
#include <commandnet.h>

int64_t test_var = 0;

cmdnet_cmd_t cmds[]   = {};
const size_t cmds_len = sizeof(cmds) / sizeof(cmds[0]);

cmdnet_var_t vars[] = {
    // {"lox_period", &lox_period}, {"lox_duty_cycle", &lox_duty_cycle},
    // {"eth_period", &eth_period}, {"eth_duty_cycle", &eth_duty_cycle},
    // {"lox_run", &lox_run},       {"eth_run", &eth_run},
    // {"lox_state", &lox_state},   {"eth_state", &eth_state},
    // {"pyro_state", &pyro_state},
    // {"time_offset_us", (int64_t*)&time_offset}
    {"test_var", &test_var}};
const size_t vars_len = sizeof(vars) / sizeof(vars[0]);
