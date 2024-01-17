import struct
import socket

struct_format = '<H6xQQq'

UDP_IP = "0.0.0.0" #localhost
UDP_PORT = 3746

try:
  recv_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) #Set up socket link
  recv_socket.bind((UDP_IP, UDP_PORT))
  print("starting listening...")

  while True:

    data, addr = recv_socket.recvfrom(128)

    id, time_us, counter, value = struct.unpack(struct_format, data)

    print(time_us)


except Exception:
  print("Error with server connection or decoding data!")

finally:
  recv_socket.close()