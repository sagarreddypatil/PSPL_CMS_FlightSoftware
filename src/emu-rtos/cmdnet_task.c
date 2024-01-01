#include "emu.h"

tcp_server_t server;
cmdnet_t cmdnet;

void cmdnet_task_init() {
  tcp_server_init(&server, &w5500, W5500_S0, 8080);
  cmdnet_init(&cmdnet, &server, cmds, cmds_len, vars, vars_len);
}
void cmdnet_task_run() {
  tcp_server_poll(&server);

  if (tcp_server_connected(&server)) {
    cmdnet_handle(&cmdnet);
  }
}