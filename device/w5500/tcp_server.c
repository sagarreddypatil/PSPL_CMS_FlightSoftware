#include <w5500/tcp_server.h>

void tcp_server_init(tcp_server_t *server, myspi_device_t *w5500,
                     w5500_socket_t socket, uint16_t port) {
    server->w5500  = w5500;
    server->socket = socket;

    myspi_lock(w5500);
    w5500_create_tcp_socket(w5500, socket, port);
    myspi_unlock(w5500);
}

void tcp_server_poll(tcp_server_t *server) {
    myspi_lock(server->w5500);
    uint8_t status = w5500_read8(server->w5500, server->socket, W5500_Sn_SR);
    myspi_unlock(server->w5500);

    myspi_lock(server->w5500);
    if (status == W5500_SOCK_CLOSED) {
        w5500_command(server->w5500, server->socket, W5500_CMD_OPEN);
    }

    if (status == W5500_SOCK_INIT) {
        w5500_command(server->w5500, server->socket, W5500_CMD_LISTEN);
    }

    if (status == W5500_SOCK_CLOSE_WAIT) {
        w5500_command(server->w5500, server->socket, W5500_CMD_DISCON);
    }
    myspi_unlock(server->w5500);
}

bool tcp_server_connected(tcp_server_t *server) {
    myspi_lock(server->w5500);
    bool connected = w5500_read8(server->w5500, server->socket, W5500_Sn_SR) ==
                     W5500_SOCK_ESTABLISHED;
    myspi_unlock(server->w5500);

    return connected;
}

tcp_client_data_t tcp_server_get_client(tcp_server_t *server) {
    tcp_client_data_t client;
    myspi_lock(server->w5500);
    w5500_read(server->w5500, server->socket, W5500_Sn_DIPR0, client.ip,
               sizeof(client.ip));
    client.port = w5500_read16(server->w5500, server->socket, W5500_Sn_DPORT0);
    myspi_unlock(server->w5500);
    return client;
}

uint16_t tcp_server_available(tcp_server_t *server) {
    myspi_lock(server->w5500);
    uint16_t available =
        w5500_read16(server->w5500, server->socket, W5500_Sn_RX_RSR0);
    myspi_unlock(server->w5500);

    return available;
}

void tcp_server_send(tcp_server_t *server, uint8_t *data, uint16_t len) {
    myspi_lock(server->w5500);
    w5500_write_data(server->w5500, server->socket, data, len);
    myspi_unlock(server->w5500);
}

size_t tcp_server_read(tcp_server_t *server, uint8_t *data, uint16_t len) {
    myspi_lock(server->w5500);
    size_t read_size =
        w5500_read_data(server->w5500, server->socket, data, len);
    myspi_unlock(server->w5500);

    return read_size;
}

void tcp_server_disconnect(tcp_server_t *server) {
    myspi_lock(server->w5500);
    w5500_command(server->w5500, server->socket, W5500_CMD_DISCON);
    myspi_unlock(server->w5500);
}
