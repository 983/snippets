//  -lws2_32 -lmswsock

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <MSWSock.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define EVENT_TYPE_RECV    1
#define EVENT_TYPE_SEND    2
#define EVENT_TYPE_CONNECT 3

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

void async_connect(SOCKET sock, struct sockaddr *addr, int addr_len){
    
    MyEvent *event = (MyEvent*)malloc(sizeof(*event));
    memset(event, 0, sizeof(*event));
    
    event->event_type = EVENT_TYPE_CONNECT;
    event->wsabuf.len = 0;
    event->wsabuf.buf = NULL;
    event->len = 0;
    event->sock = sock;
    
    // TODO which dll contains this function?
    // should be mswsock, but this does not appear to be the case on this machine
    ConnectEx(sock, addr, addr_len, NULL, 0, NULL, &event->overlapped);
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
            case EVENT_TYPE_CONNECT:{
                const char *message = "Hello, World!";
                async_send(event->sock, message, strlen(message));
            }break;
            
            case EVENT_TYPE_RECV:
                // Close socket once the echoed data is received.
                closesocket(event->sock);
            break;
            
            case EVENT_TYPE_SEND:
                async_recv(event->sock, 128);
            break;
        }
        
        free(event->wsabuf.buf);
        free(event);
    }
    
    return 0;
}

#include <time.h>

double sec(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9*t.tv_nsec;
}

int main(){
    // Initialize windows socket stuff because it does not work by default.
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    
    // Attach all client socket events to this iocp thing
    HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    // Start thread to handle recv/sends
    DWORD thread_handle;
    CreateThread(NULL, 0, event_completion_thread, iocp, 0, &thread_handle);
    
    double t = sec();

    int n = 100;
    for (int i = 0; i < n; i++){
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        
        // Subscribe iocp to socket events
        CreateIoCompletionPort((HANDLE)sock, iocp, 0, 0);
        
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8080);
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
        async_connect(sock, (struct sockaddr*)&addr, sizeof(addr));
        /*
        connect(sock, (struct sockaddr*)&addr, sizeof(addr));
        
        const char *message = "Hello, World!";
        send(sock, message, strlen(message), 0);
        
        char buf[65536];
        int n = recv(sock, buf, sizeof(buf), 0);
        
        printf("%i: %.*s\n", n, n, buf);
        
        assert(n == (int)strlen(message));
        assert(0 == memcmp(buf, message, n));
        */
    }
    
    double dt = sec() - t;
    
    printf("%f ms per request\n", dt*1000.0/n);
    
    return 0;
}
