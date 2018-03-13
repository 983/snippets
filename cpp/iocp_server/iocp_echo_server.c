// 1. Create io completion port (iocp)
// 2. Wait for clients to connect
// 3. Once a client connects, associate the client socket with the iocp.
// 4. Initiate asynchronous recv on client socket with WSARecv.
//    WSARecv will return immediately and eventually.
//    The iocp will be informed when WSARecv actually finishes.
// 5. In another thread, iocp is polled with GetQueuedCompletionStatus.
//    GetQueuedCompletionStatus will stop blocking when an operation
//    finishes.
// 6. Respond to completion by initiating an async WSASend.
//
// tl;dr
// iocp subscribes to sockets
// then socket recv/sends won't block
// iocp is polled for recv/send completions in another thread

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define EVENT_TYPE_RECV 1
#define EVENT_TYPE_SEND 2

// This struct is used to remember data about recv and send events
typedef struct {
    OVERLAPPED overlapped;
    int event_type;
    WSABUF wsabuf;
    DWORD len;
    SOCKET sock;
} MyEvent;

void async_recv(SOCKET sock, size_t n_bytes){
    char *buf = (char*)malloc(n_bytes);
    
    MyEvent *event = (MyEvent*)malloc(sizeof(*event));
    memset(event, 0, sizeof(*event));
    
    event->event_type = EVENT_TYPE_RECV;
    event->wsabuf.len = n_bytes;
    event->wsabuf.buf = buf;
    event->len = n_bytes;
    event->sock = sock;
    
    DWORD flags = 0;
    WSARecv(sock, &event->wsabuf, 1, &event->len, &flags, &event->overlapped, NULL);
}

void async_send(SOCKET sock, const void *bytes, size_t n_bytes){
    char *buf = (char*)malloc(n_bytes);
    memcpy(buf, bytes, n_bytes);
    
    MyEvent *event = (MyEvent*)malloc(sizeof(*event));
    memset(event, 0, sizeof(*event));
    
    event->event_type = EVENT_TYPE_SEND;
    event->wsabuf.len = n_bytes;
    event->wsabuf.buf = buf;
    event->len = n_bytes;
    event->sock = sock;
    
    WSASend(sock, &event->wsabuf, 1, &event->len, 0, &event->overlapped, NULL);
}

DWORD event_completion_thread(HANDLE iocp){
    
    while (1){
        DWORD n_bytes = 0;
        ULONG_PTR key = 0;
        OVERLAPPED *ptr = NULL;
        // Wait for event completion
        GetQueuedCompletionStatus(iocp, &n_bytes, &key, &ptr, INFINITE);
        
        // Data associated with event
        MyEvent *event = (MyEvent*)ptr;
        
        // Find out which kind of event finished
        switch (event->event_type){
            case EVENT_TYPE_RECV:
                // Echo back received data
                async_send(event->sock, event->wsabuf.buf, n_bytes);
            break;
            
            case EVENT_TYPE_SEND:
                // Close socket once the echoed data is sent.
                closesocket(event->sock);
            break;
        }
        
        free(event->wsabuf.buf);
        free(event);
    }
    
    return 0;
}

int main(){
    // Initialize windows socket stuff because it does not work by default.
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    
    // Create server socket
    const char *port = "8080";
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *result = NULL;
    getaddrinfo(NULL, port, &hints, &result);
    SOCKET server_sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    assert(INVALID_SOCKET != server_sock);
    
    // The port will still be occupied if we close the server.
    // Make it not so.
    char enable = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    
    // Bind server socket to port
    int err = bind(server_sock, result->ai_addr, (int)result->ai_addrlen);
    
    if (0 != err){
        printf("Failed to bind socket to port %s, probably already in use.\n", port);
        return 0;
    }
    
    freeaddrinfo(result);
    
    listen(server_sock, SOMAXCONN);

    // Attach all client socket events to this iocp thing
    HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    // Start thread to handle recv/sends
    DWORD thread_handle;
    CreateThread(NULL, 0, event_completion_thread, iocp, 0, &thread_handle);
    
    while (1){
        // Accept client request
        SOCKET client_sock = accept(server_sock, NULL, NULL);
        
        // Subscribe iocp to client socket events
        CreateIoCompletionPort((HANDLE)client_sock, iocp, 0, 0);
        
        // Initiate first recv
        async_recv(client_sock, 65536);
    }
    
    // cleanup if I ever want to write code to shutdown server
    closesocket(server_sock);
    
    WSACleanup();
    
    return 0;
}
