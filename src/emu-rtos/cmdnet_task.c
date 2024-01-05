#include "emu.h"

tcp_server_t server;
cmdnet_t cmdnet;

void cmdnet_task_entrypoint() {
    tcp_server_init(&server, &eth0, COMMANDNET_SOCKET, COMMANDNET_PORT);
    cmdnet_init(&cmdnet, &server, cmds, cmds_len, vars, vars_len);

    while (true) {
        tcp_server_poll(&server);

        if (tcp_server_connected(&server)) {
            cmdnet_handle(&cmdnet);
        }
    }
}