#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>

#include "base_server.h"
#include "logger.h"

namespace queueing_service {
    base_server::base_server() {
        m_listening_socket = NULL;
        m_stop = false;
    }

    int base_server::start(unsigned short t_port) {
        sockaddr_in serverAddress{};
        int sockAddrLen = sizeof(struct sockaddr);
        WSADATA wsaData;

        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            LOG_ERROR("BASE_SERVER_START", "WSAStartup failed with error: %d", result);
            return -1;
        }

        memset((char*)&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(t_port);

        this->m_listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (this->m_listening_socket == INVALID_SOCKET) {
            LOG_ERROR("BASE_SERVER_START", "Creating socket failed with error: %d", WSAGetLastError());
            WSACleanup();
            return -1;
        }

        if (bind(this->m_listening_socket, (LPSOCKADDR)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
            LOG_ERROR("BASE_SERVER_START", "bind failed with error: %d", WSAGetLastError());
            closesocket(this->m_listening_socket);
            WSACleanup();
            return -1;
        }

        unsigned long int nonBlockingMode = 1;
        if (ioctlsocket(this->m_listening_socket, FIONBIO, &nonBlockingMode) == SOCKET_ERROR) {
            LOG_ERROR("BASE_SERVER_START", "ioctlsocket failed with error: %d", WSAGetLastError());
            closesocket(this->m_listening_socket);
            WSACleanup();
            return -1;
        }

        if (listen(this->m_listening_socket, SOMAXCONN) == SOCKET_ERROR) {
            LOG_ERROR("BASE_SERVER_START", "listen failed with error: %d", WSAGetLastError());
            closesocket(this->m_listening_socket);
            WSACleanup();
            return -1;
        }

        m_process_accept = std::thread(&base_server::accept_connections, this);
        return 0;
    }

    void base_server::stop() {
        m_stop = true;

        if (m_process_accept.joinable())
            m_process_accept.join();

        closesocket(m_listening_socket);
    }

    void base_server::accept_connections() {
        struct timeval timeout {};
        struct fd_set fds {};

        timeout.tv_sec = 0;
        timeout.tv_usec = 50;

        while (!m_stop) {
            FD_ZERO(&fds);
            FD_SET(this->m_listening_socket, &fds);

            switch (select(0, &fds, 0, 0, &timeout)) {
                case 0: {  // timeout
                    break;
                };
                case -1: {  // error
                    LOG_ERROR("BASE_SERVER_ACCEPT_CONNECTIONS", "select failed with error: %d", WSAGetLastError());
                    break;
                };
                default: {  // success
                    struct sockaddr_storage connected_service_addr {};
                    int connected_service_addr_len = sizeof(connected_service_addr);

                    SOCKET connected_service_socket = accept(this->m_listening_socket, NULL, NULL);
                    getpeername(connected_service_socket, (struct sockaddr*)&connected_service_addr, &connected_service_addr_len);

                    struct sockaddr_in* s = (struct sockaddr_in*)&connected_service_addr;

                    LOG_INFO("BASE_SERVER_ACCEPT_CONNECTIONS", "Service connected from %s:%d", inet_ntoa(s->sin_addr), ntohs(s->sin_port));
                    handle_accept(&connected_service_socket);
                    break;
                };
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}