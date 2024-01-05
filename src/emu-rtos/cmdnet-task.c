#include "emu.h"

bool test_handler(mpack_reader_t *reader) {
    safeprintf("Test handler called!\n");
    return true;
}

const cmdnet_endpoint_t endpoints[] = {
    CMDNET_HANDLER(test),
};

const size_t n_endpoints = sizeof(endpoints) / sizeof(cmdnet_endpoint_t);

tcp_server_t server;
cmdnet_t cmdnet;

void cmdnet_task_main() {
    tcp_server_init(&server, &eth0, COMMANDNET_SOCKET, COMMANDNET_PORT);
    cmdnet2_init(&cmdnet, &server, endpoints, n_endpoints);

    while (true) {
        tcp_server_poll(&server);

        if (tcp_server_connected(&server)) {
            cmdnet2_handle(&cmdnet);
        }
    }
}