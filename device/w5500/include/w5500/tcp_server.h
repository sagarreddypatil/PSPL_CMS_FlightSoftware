#pragma once
#include <myspi.h>
#include <w5500.h>

typedef struct {
  myspi_device_t *w5500;
  w5500_socket_t socket;
} tcp_server_t;

typedef struct {
  ip_t ip;
  uint16_t port;
} tcp_client_data_t;

void tcp_server_init(tcp_server_t *server, myspi_device_t *w5500,
                     w5500_socket_t socket, uint16_t port);
void tcp_server_poll(tcp_server_t *server);

bool tcp_server_connected(tcp_server_t *server);
tcp_client_data_t tcp_server_get_client(tcp_server_t *server);

uint16_t tcp_server_available(tcp_server_t *server);
void tcp_server_send(tcp_server_t *server, uint8_t *data, uint16_t len);
size_t tcp_server_read(tcp_server_t *server, uint8_t *data, uint16_t len);

void tcp_server_disconnect(tcp_server_t *server);
