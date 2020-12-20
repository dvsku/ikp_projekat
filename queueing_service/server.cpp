#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "server.h"
#include <stdio.h>
#include <thread>

#define ACCESS_BUFFER_SIZE 1024
#define IP_ADDRESS_LEN 16

server::server(int port) {
    m_port = port;
    m_listening_socket = NULL;
    m_connected_service_socket = NULL;
}

int server::start() {
    sockaddr_in serverAddress;
    int sockAddrLen = sizeof(struct sockaddr);
    char accessBuffer[ACCESS_BUFFER_SIZE];
    int iResult;
    WSADATA wsaData;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    memset((char*)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET; /*set server address protocol family*/
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(this->m_port);

    this->m_listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (this->m_listening_socket == INVALID_SOCKET) {
        wprintf(L"creating socket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    if (bind(this->m_listening_socket, (LPSOCKADDR)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        wprintf(L"bind failed with error: %d\n", WSAGetLastError());
        closesocket(this->m_listening_socket);
        WSACleanup();
        return 1;
    }

    unsigned long int nonBlockingMode = 1;
    if (ioctlsocket(this->m_listening_socket, FIONBIO, &nonBlockingMode) == SOCKET_ERROR) {
        wprintf(L"ioctlsocket failed with error: %d\n", WSAGetLastError());
        closesocket(this->m_listening_socket);
        WSACleanup();
        return 1;
    }

    if (listen(this->m_listening_socket, SOMAXCONN) == SOCKET_ERROR) {
        wprintf(L"listen failed with error: %d\n", WSAGetLastError());
        closesocket(this->m_listening_socket);
        WSACleanup();
        return 1;
    }

    printf("Simple TCP server started and waiting clients.\n");
    
    std::thread tSelect(&server::do_accept, this);
    tSelect.join();
}

void server::do_accept() {
    struct timeval timeout;
    struct fd_set fds;

    timeout.tv_sec = 0;
    timeout.tv_usec = 50;
    
    while (true) {
        FD_ZERO(&fds);
        FD_SET(this->m_listening_socket, &fds);

        switch (select(0, &fds, 0, 0, &timeout)) {
            case 0 : {  // timeout
                //wprintf(L"select timed out");
                break;
            };
            case -1: {  // error
                wprintf(L"select failed with error: %d\n", WSAGetLastError());
                break;
            };
            default: {  // success
                struct sockaddr_storage connected_service_addr;
                int connected_service_addr_len = sizeof(connected_service_addr);
                
                this->m_connected_service_socket = accept(this->m_listening_socket, NULL, NULL);
                getpeername(this->m_connected_service_socket, (struct sockaddr*)&connected_service_addr, &connected_service_addr_len);

                struct sockaddr_in* s = (struct sockaddr_in*)&connected_service_addr;

                printf("service connected from %s:%d\n", inet_ntoa(s->sin_addr), ntohs(s->sin_port));
                handle_accept();
                break;
            };
        }
    }
}