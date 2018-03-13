import socket
import time
import concurrent.futures

def make_request():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    sock.connect(("127.0.0.1", 8080))

    message = b"Hello, World!"
    sock.send(message)
    response = sock.recv(65536)
    sock.close()
    assert(message == response)

executor = concurrent.futures.ThreadPoolExecutor(4)

for _ in range(10):
    t = time.clock()

    n = 100

    futures = [executor.submit(make_request) for _ in range(n)]

    concurrent.futures.wait(futures)
    
    dt = time.clock() - t

    print("%f ms per request"%(dt*1000/n))
