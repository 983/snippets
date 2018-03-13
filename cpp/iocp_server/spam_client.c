#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

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

    double t = sec();

    int n = 100;
    for (int i = 0; i < n; i++){
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8080);
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
        connect(sock, (struct sockaddr*)&addr, sizeof(addr));
        
        const char *message = "Hello, World!";
        send(sock, message, strlen(message), 0);
        
        char buf[65536];
        int n = recv(sock, buf, sizeof(buf), 0);
        
        printf("%i: %.*s\n", n, n, buf);
        
        assert(n == (int)strlen(message));
        assert(0 == memcmp(buf, message, n));
        
        closesocket(sock);
    }
    
    double dt = sec() - t;
    
    printf("%f ms per request\n", dt*1000.0/n);
    
    return 0;
}
