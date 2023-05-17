#include <commandnet/mpack_socket.h>

void mpack_socket_writer_flush(mpack_writer_t* writer, const char* buf,
                               size_t len) {
  tcp_server_t* server = (tcp_server_t*)mpack_writer_context(writer);
  tcp_server_send(server, (uint8_t*)buf, len);
}

void mpack_socket_writer_teardown(mpack_writer_t* writer) {
  tcp_server_disconnect((tcp_server_t*)mpack_writer_context(writer));

  writer->buffer  = NULL;
  writer->context = NULL;
}

void mpack_writer_init_socket(mpack_writer_t* writer, tcp_server_t* server,
                              char* buf, size_t len) {
  mpack_writer_init(writer, buf, len);
  mpack_writer_set_context(writer, server);
  mpack_writer_set_flush(writer, mpack_socket_writer_flush);
  mpack_writer_set_teardown(writer, mpack_socket_writer_teardown);
}

size_t mpack_socket_reader_fill(mpack_reader_t* reader, char* buf, size_t len) {
  tcp_server_t* server = (tcp_server_t*)mpack_reader_context(reader);
  return tcp_server_read(server, (uint8_t*)buf, len);
}

void mpack_socket_reader_skip(mpack_reader_t* reader, size_t len) {
  tcp_server_t* server = (tcp_server_t*)mpack_reader_context(reader);

  char _;
  for (size_t i = 0; i < len; i++) {
    if (tcp_server_read(server, (uint8_t*)&_, 1) == 0) break;
  }
}

void mpack_socket_reader_teardown(mpack_reader_t* reader) {
  reader->buffer  = NULL;
  reader->context = NULL;
}

void mpack_reader_init_socket(mpack_reader_t* reader, tcp_server_t* server,
                              char* buf, size_t len) {
  mpack_reader_init(reader, buf, len, 0);
  mpack_reader_set_context(reader, server);
  mpack_reader_set_fill(reader, mpack_socket_reader_fill);
  mpack_reader_set_skip(reader, mpack_socket_reader_skip);
  mpack_reader_set_teardown(reader, mpack_socket_reader_teardown);
}