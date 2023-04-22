#include <commandnet.h>
#include <commandnet/mpack_socket.h>

void cmdnet_init(cmdnet_t* cmdnet, cmdnet_cmd_t* cmds, size_t cmds_len,
                 cmdnet_var_t* vars, size_t vars_len, tcp_server_t* socket) {
  cmdnet->cmds     = cmds;
  cmdnet->cmds_len = cmds_len;
  cmdnet->vars     = vars;
  cmdnet->vars_len = vars_len;
  cmdnet->socket   = socket;
}

cmdnet_status_t cmdnet_exec(cmdnet_t* cmdnet, const char* name) {
  for (int i = 0; i < cmdnet->cmds_len; i++) {
    if (strcmp(cmdnet->cmds[i].name, name) == 0) {
      return cmdnet->cmds[i].handler();
    }
  }
  return CMD_NOTFOUND;
}

cmdnet_status_t cmdnet_set_var(cmdnet_t* cmdnet, const char* name,
                               int64_t* oldval, int64_t newval) {
  for (int i = 0; i < cmdnet->vars_len; i++) {
    if (strcmp(cmdnet->vars[i].name, name) == 0) {
      *oldval                 = *cmdnet->vars[i].valptr;
      *cmdnet->vars[i].valptr = newval;
      return CMD_SUCCESS;
    }
  }

  return CMD_NOTFOUND;
}

cmdnet_status_t cmdnet_get_var(cmdnet_t* cmdnet, const char* name,
                               int64_t* val) {
  for (int i = 0; i < cmdnet->vars_len; i++) {
    if (strcmp(cmdnet->vars[i].name, name) == 0) {
      *val = *cmdnet->vars[i].valptr;
      return CMD_SUCCESS;
    }
  }

  return CMD_NOTFOUND;
}

void cmdnet_handle(cmdnet_t* cmdnet) {
  char buf[MPACK_BUFFER_SIZE];  // don't expect this to take much more

  // parse the command
  mpack_reader_t reader;
  mpack_reader_init_socket(&reader, cmdnet->socket, buf, sizeof(buf));

  mpack_expect_array_max(&reader, 3);
  cmdnet_req_t req = mpack_expect_u8(&reader);

  mpack_writer_t writer;
  mpack_writer_init_socket(&writer, cmdnet->socket, buf, sizeof(buf));

  if (req == EXEC_CMD) {
    char name[64];
    mpack_expect_cstr(&reader, name, sizeof(name));

    mpack_reader_destroy(&reader);

    cmdnet_status_t status = cmdnet_exec(cmdnet, name);

    mpack_start_array(&writer, 1);
    mpack_write_u8(&writer, status);
    mpack_finish_array(&writer);
  } else if (req == ALL_CMDS) {
    mpack_reader_destroy(&reader);

    cmdnet_status_t status = CMD_SUCCESS;
    cmdnet_cmd_t* cmds     = cmdnet->cmds;
    size_t cmds_len        = cmdnet->cmds_len;

    mpack_start_array(&writer, 2);
    mpack_write_u8(&writer, status);
    mpack_start_array(&writer, cmds_len);
    for (size_t i = 0; i < cmds_len; i++) {
      mpack_write_cstr(&writer, cmds[i].name);
    }
    mpack_finish_array(&writer);
    mpack_finish_array(&writer);
  } else if (req == SET_VAR) {
    char name[64];
    int64_t newval;

    mpack_expect_cstr(&reader, name, sizeof(name));
    newval = mpack_expect_i64(&reader);

    mpack_reader_destroy(&reader);

    int64_t oldval         = 0xdeadbeef;
    cmdnet_status_t status = cmdnet_set_var(cmdnet, name, &oldval, newval);

    mpack_start_array(&writer, 2);
    mpack_write_u8(&writer, status);
    mpack_write_i64(&writer, oldval);
    mpack_finish_array(&writer);
  } else if (req == GET_VAR) {
    char name[64];
    mpack_expect_cstr(&reader, name, sizeof(name));
    mpack_reader_destroy(&reader);

    int64_t val            = 0xdeadbeef;
    cmdnet_status_t status = cmdnet_get_var(cmdnet, name, &val);

    mpack_start_array(&writer, 2);
    mpack_write_u8(&writer, status);
    mpack_write_i64(&writer, val);
    mpack_finish_array(&writer);
  } else if (req == ALL_VARS) {
    mpack_reader_destroy(&reader);

    cmdnet_status_t status = CMD_SUCCESS;
    cmdnet_var_t* vars     = cmdnet->vars;
    size_t vars_len        = cmdnet->vars_len;

    mpack_start_array(&writer, 2);
    mpack_write_u8(&writer, status);
    mpack_start_array(&writer, vars_len);
    for (size_t i = 0; i < vars_len; i++) {
      mpack_write_cstr(&writer, vars[i].name);
      mpack_write_i64(&writer, *(vars[i].valptr));
    }
    mpack_finish_array(&writer);
    mpack_finish_array(&writer);
  } else {
    mpack_reader_destroy(&reader);

    mpack_start_array(&writer, 1);
    mpack_write_u8(&writer, CMD_INVALID);
    mpack_finish_array(&writer);
  }

  mpack_writer_destroy(&writer);
}