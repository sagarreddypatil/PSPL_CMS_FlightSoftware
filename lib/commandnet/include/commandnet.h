/** @file commandnet.h
 *  @brief CommandNet protocol implementation
 *
 * @author Sagar R. Patil (sagarreddypatil)
 *
 */

#pragma once
#include <mpack.h>
#include <stdint.h>
#include <w5500/tcp_server.h>

/**
 * @defgroup commandnet CommandNet
 * @brief CommandNet protocol implementation
 * Request-response protocol for sending commands over a TCP socket
 * Request format:
 * [request type(u8), args...]
 * EXEC_CMD: [EXEC_CMD, command name]
 * ALL_CMDS: [ALL_CMDS]
 * SET_VAR: [SET_VAR, variable name, value]
 * GET_VAR: [GET_VAR, variable name]
 * ALL_VARS: [ALL_VARS]
 *
 * Response format:
 * [status(u8), args...]
 * EXEC_CMD: [status(SUCCESS/ERROR)]
 * ALL_CMDS: [status(SUCCESS/ERROR), [command 0 name, ...]]
 * SET_VAR: [status(SUCCESS/ERROR), old value]
 * GET_VAR: [status(SUCCESS/ERROR), value]
 * ALL_VARS: [status(SUCCESS/ERROR), [[var 0 name, var 0 value], ...]]
 * @{
 */

typedef enum {
    EXEC_CMD = 1,
    ALL_CMDS,
    SET_VAR,
    GET_VAR,
    ALL_VARS,
} cmdnet_req_t;

typedef enum {
    CMD_SUCCESS  = 0,
    CMD_NOTFOUND = -1,
    CMD_INVALID  = -2,
    CMD_ERROR    = -3,
} cmdnet_status_t;

typedef struct {
    const char* name;
    cmdnet_status_t (*handler)();
} cmdnet_cmd_t;

#define CMDNET_COMMAND(name)                                           \
    cmdnet_status_t cmdnet_handle_##name();                            \
    const cmdnet_cmd_t cmdnet_##name = {#name, &cmdnet_handle_##name}; \
    cmdnet_status_t cmdnet_handle_##name()

typedef struct {
    const char* name;
    int64_t* valptr;
} cmdnet_var_t;

#define CMDNET_VAR(name, val)               \
    int64_t name                     = val; \
    const cmdnet_var_t cmdnet_##name = {#name, &name};

typedef int (*cmdnet_io_func_t)(void* data, size_t len);

typedef struct {
    cmdnet_cmd_t* cmds;
    size_t cmds_len;

    cmdnet_var_t* vars;
    size_t vars_len;

    tcp_server_t* socket;
} cmdnet_t;

void cmdnet_init(cmdnet_t* cmdnet, tcp_server_t* socket, cmdnet_cmd_t* cmds,
                 size_t cmds_len, cmdnet_var_t* vars, size_t vars_len);

void cmdnet_handle(cmdnet_t* cmdnet);

/**
 * @}
 */