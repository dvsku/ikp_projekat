#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>

#include "base_server.h"
#include "./helpers/logger.h"

namespace queueing_service {
    base_server::base_server() {
        m_listening_socket = NULL;
        m_stop = false;
    }

    /// <summary>
    /// Open a listening socket and start a thread to accept connections.
    /// </summary>
    /// <param name="t_port">Port</param>
    /// <returns>-1 if an error occurred, 0 if it was successful.</returns>
    int base_server::start(unsigned short t_port) {
        sockaddr_in serverAddress{};
        int sockAddrLen = sizeof(struct sockaddr);
        WSADATA wsaData;

        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            LOG_ERROR("BS_START", "WSAStartup failed with error: %d", result);
            return -1;
        }

        memset((char*)&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(t_port);

        this->m_listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (this->m_listening_socket == INVALID_SOCKET) {
            LOG_ERROR("BS_START", "Creating socket failed with error: %d", WSAGetLastError());
            WSACleanup();
            return -1;
        }

        if (bind(this->m_listening_socket, (LPSOCKADDR)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
            LOG_ERROR("BS_START", "bind failed with error: %d", WSAGetLastError());
            closesocket(this->m_listening_socket);
            WSACleanup();
            return -1;
        }

        unsigned long int nonBlockingMode = 1;
        if (ioctlsocket(this->m_listening_socket, FIONBIO, &nonBlockingMode) == SOCKET_ERROR) {
            LOG_ERROR("BS_START", "ioctlsocket failed with error: %d", WSAGetLastError());
            closesocket(this->m_listening_socket);
            WSACleanup();
            return -1;
        }

        if (listen(this->m_listening_socket, SOMAXCONN) == SOCKET_ERROR) {
            LOG_ERROR("BS_START", "listen failed with error: %d", WSAGetLastError());
            closesocket(this->m_listening_socket);
            WSACleanup();
            return -1;
        }

        m_process_accept = std::thread(&base_server::accept_connections, this);
        return 0;
    }

    /// <summary>
    /// Close the listening socket and stop accept connections thread.
    /// </summary>
    void base_server::stop() {
        m_stop = true;

        if (m_process_accept.joinable())
            m_process_accept.join();

        closesocket(m_listening_socket);
    }

    /// <summary>
    /// Checks the listening socket for incoming connections and accepts them.
    /// </summary>
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
                    LOG_ERROR("BS_AC", "select failed with error: %d", WSAGetLastError());
                    break;
                };
                default: {  // success
                    SOCKET connected_service_socket = accept(this->m_listening_socket, NULL, NULL);
                    handle_accept(connected_service_socket);
                    break;
                };
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}