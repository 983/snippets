import socket
import random, time, sys, os

host = "localhost"
port = 12345

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.bind((host, port))
server_socket.listen(5)

while True:
    client_socket, client_address = server_socket.accept()
    
    print("address:", client_address)
    
    buffer = b""

    while True:
        data = client_socket.recv(65536)

        if len(data) == 0:
            break
        
        print("data:", data)
        buffer += data

        end = b"\r\n\r\n"
        i = buffer.find(end)

        if i != -1:
            if i + len(end) != len(buffer):
                print("Data after header")
                print(buffer)
            break

    header = """HTTP/1.0 200 OK
Content-Type: text/html
Content-Length: %d

"""
    
    body = """<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>Title goes here</title>
</head>
<body>
<p>Your number is: %d</p>
</body>
</html>
"""%random.getrandbits(128)

    body = body.encode("utf-8")
    header = header%len(body)
    header = header.encode("utf-8")

    client_socket.send(header + body)

    client_socket.close()
