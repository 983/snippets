import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

sock.connect(("127.0.0.1", 8080))

message = b"Hello, World!"
sock.send(message)
response = sock.recv(65536)
print(response)
sock.close()
