#include "emu.h"

tcp_server_t server;
cmdnet_t cmdnet;

void cmdnet_task_main() {
    tcp_server_init(&server, &eth0, COMMANDNET_SOCKET, COMMANDNET_PORT);
    cmdnet2_init(&cmdnet, &server, endpoints, n_endpoints);

    TickType_t prev_wake = xTaskGetTickCount();

    while (true) {
        tcp_server_poll(&server);

        if (tcp_server_connected(&server)) {
            cmdnet2_handle(&cmdnet);
        }
        vTaskDelayUntil(&prev_wake, pdMS_TO_TICKS(BB_LOOP_PERIOD));
    }
}