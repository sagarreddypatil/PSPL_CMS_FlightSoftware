#include "emu.h"

int64_t test_var = 0;

cmdnet_cmd_t cmds[]   = {};
const size_t cmds_len = sizeof(cmds) / sizeof(cmds[0]);

cmdnet_var_t vars[]   = {{"test_var", &test_var}};
const size_t vars_len = sizeof(vars) / sizeof(vars[0]);

tcp_server_t server;
cmdnet_t cmdnet;

void cmdnet_task_main() {
    tcp_server_init(&server, &eth0, COMMANDNET_SOCKET, COMMANDNET_PORT);
    cmdnet_init(&cmdnet, &server, cmds, cmds_len, vars, vars_len);

    while (true) {
        tcp_server_poll(&server);

        if (tcp_server_connected(&server)) {
            cmdnet_handle(&cmdnet);
        }
    }
}