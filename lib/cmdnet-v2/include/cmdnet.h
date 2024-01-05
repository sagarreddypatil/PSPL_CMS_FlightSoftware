#pragma once

#include <mpack.h>
#include <w5500/tcp_server.h>

#define CMDNET_ENDPOINT_NAME_LENGTH 32

typedef bool (*cmdnet_handler_t)(mpack_reader_t* reader);

typedef struct {
    const char* name;
    cmdnet_handler_t handler;
} cmdnet_endpoint_t;

typedef struct {
    tcp_server_t* server;

    const cmdnet_endpoint_t* endpoints;
    size_t n_endpoints;
} cmdnet_t;

void cmdnet2_init(cmdnet_t* cmdnet, tcp_server_t* server,
                  const cmdnet_endpoint_t* endpoints, size_t n_endpoints);
void cmdnet2_handle(cmdnet_t* cmdnet);
