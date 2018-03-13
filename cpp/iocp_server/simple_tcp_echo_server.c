#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(){
    // Initialize windows socket stuff because it does not work by default.
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    
    
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    
    struct addrinfo *result = NULL;
    const char *port = "8080";
    getaddrinfo(NULL, port, &hints, &result);
    
    SOCKET server_sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    
    assert(INVALID_SOCKET != server_sock);
    
    char enable = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    
    int err = bind(server_sock, result->ai_addr, (int)result->ai_addrlen);
    
    freeaddrinfo(result);
    
    printf("err: %i\n", err);
    
    listen(server_sock, SOMAXCONN);
    
    while (1){      
        SOCKET client_sock = accept(server_sock, NULL, NULL);
        
        char buf[65536];
        int n = recv(client_sock, buf, sizeof(buf), 0);
        if (n > 0){
            //fwrite(buf, 1, n, stdout);
            send(client_sock, buf, n, 0);
        }
        
        closesocket(client_sock);
    }
    
    closesocket(server_sock);
    
    WSACleanup();
    
    return 0;
}
