#include "emu.h"
#include <commandnet.h>

cmdnet_cmd_t cmds[]   = {};
const size_t cmds_len = sizeof(cmds) / sizeof(cmds[0]);

cmdnet_var_t vars[] = {
    // {"lox_period", &lox_period}, {"lox_duty_cycle", &lox_duty_cycle},
};
const size_t vars_len = sizeof(vars) / sizeof(vars[0]);
