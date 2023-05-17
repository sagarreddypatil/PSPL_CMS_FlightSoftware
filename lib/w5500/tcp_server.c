#include <w5500/tcp_server.h>

void tcp_server_init(tcp_server_t *server, const spi_device_t *w5500,
                     w5500_socket_t socket, uint16_t port) {
  server->w5500  = w5500;
  server->socket = socket;

  w5500_create_tcp_socket(w5500, socket, port);
}

void tcp_server_poll(tcp_server_t *server) {
  uint8_t status = w5500_read8(server->w5500, server->socket, W5500_Sn_SR);
  if (status == W5500_SOCK_CLOSED) {
    w5500_command(server->w5500, server->socket, W5500_CMD_OPEN);
  }

  if (status == W5500_SOCK_INIT) {
    w5500_command(server->w5500, server->socket, W5500_CMD_LISTEN);
  }

  if (status == W5500_SOCK_CLOSE_WAIT) {
    w5500_command(server->w5500, server->socket, W5500_CMD_DISCON);
  }
}

bool tcp_server_connected(tcp_server_t *server) {
  return w5500_read8(server->w5500, server->socket, W5500_Sn_SR) ==
         W5500_SOCK_ESTABLISHED;
}

tcp_client_data_t tcp_server_get_client(tcp_server_t *server) {
  tcp_client_data_t client;
  w5500_read(server->w5500, server->socket, W5500_Sn_DIPR0, client.ip,
             sizeof(client.ip));
  client.port = w5500_read16(server->w5500, server->socket, W5500_Sn_DPORT0);
  return client;
}

uint16_t tcp_server_available(tcp_server_t *server) {
  return w5500_read16(server->w5500, server->socket, W5500_Sn_RX_RSR0);
}

void tcp_server_send(tcp_server_t *server, uint8_t *data, uint16_t len) {
  w5500_write_data(server->w5500, server->socket, data, len);
}

size_t tcp_server_read(tcp_server_t *server, uint8_t *data, uint16_t len) {
  return w5500_read_data(server->w5500, server->socket, data, len);
}

void tcp_server_disconnect(tcp_server_t *server) {
  w5500_command(server->w5500, server->socket, W5500_CMD_DISCON);
}
