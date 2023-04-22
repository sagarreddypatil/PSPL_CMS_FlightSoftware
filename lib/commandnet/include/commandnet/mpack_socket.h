#include <mpack.h>
#include <w5500/tcp_server.h>

void mpack_writer_init_socket(mpack_writer_t* writer, tcp_server_t* server,
                              char* buf, size_t len);

void mpack_reader_init_socket(mpack_reader_t* reader, tcp_server_t* server,
                              char* buf, size_t len);