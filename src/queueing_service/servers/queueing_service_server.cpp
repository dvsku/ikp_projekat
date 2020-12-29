#include "queueing_service_server.h"
#include "../queueing_service.h"
#include "logger.h"

#define BUFFER_SIZE 1024

namespace queueing_service {
    queueing_service_server::queueing_service_server(queueing_service* t_parent) : 
        base_server::base_server(), service_message_handler::service_message_handler(t_parent) {

        m_connected_service_socket = nullptr;
        m_parent = t_parent;
    }

    void queueing_service_server::stop() {
        // close listening socket and stop its thread
        base_server::stop();

        // wait for recv thread to stop
        if (m_connected_service_recv.joinable())
            m_connected_service_recv.join();
    }

    int queueing_service_server::start(unsigned short t_port) {
        auto ret_val = base_server::start(t_port);
        m_connected_service_recv = std::thread(&queueing_service_server::recv_from_connected_service, this);
        return ret_val;
    }

    void queueing_service_server::handle_accept(SOCKET* t_socket) {
        // close listening socket TODO

        // set connected service socket
        if (t_socket != nullptr) {
            m_connected_service_socket = t_socket;
        }
    }

    int queueing_service_server::send_message(char* t_msg, unsigned int t_len) {
        if (m_connected_service_socket != nullptr) {
            int ret_val = send(*m_connected_service_socket, t_msg, t_len, 0);
            if (ret_val == SOCKET_ERROR) {
                LOG_ERROR("QSS_SM", "send failed with error: %d", WSAGetLastError());
                return -1;
            }
            else {
                return ret_val;
            }
        }
        return -1;
    }

    void queueing_service_server::recv_from_connected_service() {
        struct timeval timeout {};
        struct fd_set fds {};

        timeout.tv_sec = 0;
        timeout.tv_usec = 50;

        while (!m_stop) {
            if (m_connected_service_socket == nullptr) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            FD_ZERO(&fds);
            FD_SET(*m_connected_service_socket, &fds);

            switch (select(0, &fds, 0, 0, &timeout)) {
                case 0: {  // timeout
                    break;
                };
                case -1: {  // error
                    LOG_ERROR("QSS_RECV", "select failed with error: %d", WSAGetLastError());
                    break;
                };
                default: {  // success
                    char* recv_buffer = new char[BUFFER_SIZE];

                    int bytes_received = recv(*m_connected_service_socket, recv_buffer, BUFFER_SIZE, 0);

                    if (bytes_received == SOCKET_ERROR) {
                        closesocket(*m_connected_service_socket);
                        m_connected_service_socket = nullptr;
                        m_parent->on_service_disconnected();
                        LOG_INFO("QSS_RECV", "Queueing service disconnected.", WSAGetLastError());
                        delete[] recv_buffer;
                        break;
                    }

                    if (bytes_received == 0) {
                        closesocket(*m_connected_service_socket);
                        m_connected_service_socket = nullptr;
                        m_parent->on_service_disconnected();
                        LOG_INFO("QSS_RECV", "Queueing service disconnected.", WSAGetLastError());
                        delete[] recv_buffer;
                        break;
                    }

                    handle_message(recv_buffer, bytes_received);
                    break;
                };
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}