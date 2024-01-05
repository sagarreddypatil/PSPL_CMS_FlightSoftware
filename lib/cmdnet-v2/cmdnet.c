#include <cmdnet.h>
#include <mpack_socket.h>

#ifndef NDEBUG

void safeprintf(const char* format, ...);
#define DEBUG_PRINT(...) safeprintf("CmdNet: " __VA_ARGS__)

#else

#define DEBUG_PRINT(...)

#endif

void cmdnet2_init(cmdnet_t* cmdnet, tcp_server_t* server,
                  const cmdnet_endpoint_t* endpoints, size_t n_endpoints) {
    cmdnet->server      = server;
    cmdnet->endpoints   = endpoints;
    cmdnet->n_endpoints = n_endpoints;
}

void cmdnet2_handle(cmdnet_t* cmdnet) {
    tcp_server_t* server = cmdnet->server;
    if (!tcp_server_available(server)) {
        return;
    }

    char buf[256];  // don't expect this to take much more

    // parse the command
    mpack_reader_t _reader;
    mpack_reader_t* reader = &_reader;
    mpack_reader_init_socket(reader, cmdnet->server, buf, sizeof(buf));

#define CHECK_ERR                                 \
    if (mpack_reader_error(reader) != mpack_ok) { \
        DEBUG_PRINT("Invalid Request\n");         \
        tcp_server_disconnect(server);            \
        return;                                   \
    }

    uint32_t len = mpack_expect_array(reader);
    CHECK_ERR;

    char endpoint_name[CMDNET_ENDPOINT_NAME_LENGTH];
    mpack_expect_cstr(reader, endpoint_name, CMDNET_ENDPOINT_NAME_LENGTH);
    CHECK_ERR;
#undef CHECK_ERR

    bool response = false;
    bool found    = false;

    for (int i = 0; i < cmdnet->n_endpoints; i++) {
        const cmdnet_endpoint_t* endpoint = &cmdnet->endpoints[i];
        if (strcmp(endpoint->name, endpoint_name) != 0) continue;

        response = endpoint->handler(reader);
        found    = true;
    }

    if (found) {
        DEBUG_PRINT("Handled, response: %s\n", response ? "true" : "false");
    } else {
        DEBUG_PRINT("\"%s\" not found\n", endpoint_name);
    }

    mpack_reader_destroy(reader);

    mpack_writer_t _writer;
    mpack_writer_t* writer = &_writer;
    mpack_writer_init_socket(writer, server, buf, sizeof(buf));

    mpack_write_bool(writer, response);
    mpack_writer_destroy(writer);
}
