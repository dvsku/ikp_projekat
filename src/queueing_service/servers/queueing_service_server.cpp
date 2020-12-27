#include "queueing_service_server.h"
#include "../queueing_service.h"
#include "common_enums.h"
#include "logger.h"
#include "binary_reader.h"

#define BUFFER_SIZE 1024

namespace queueing_service {
    queueing_service_server::queueing_service_server(queueing_service* t_parent) : base_server::base_server() {
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
                LOG_ERROR("QUEUEING_SERVICE_SERVER_SEND_MESSAGE", "send failed with error: %d", WSAGetLastError());
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
                    LOG_ERROR("QUEUEING_SERVICE_SERVER_RECV_FROM_CONNECTED_SERVICE", "select failed with error: %d", WSAGetLastError());
                    break;
                };
                default: {  // success
                    char* recv_buffer = new char[BUFFER_SIZE];

                    int bytes_received = recv(*m_connected_service_socket, recv_buffer, BUFFER_SIZE, 0);

                    if (bytes_received == SOCKET_ERROR) {
                        closesocket(*m_connected_service_socket);
                        m_connected_service_socket = nullptr;
                        LOG_INFO("QUEUEING_SERVICE_SERVER_RECV_FROM_CONNECTED_SERVICE", "Queueing service disconnected.", WSAGetLastError());
                        delete[] recv_buffer;
                        break;
                    }

                    if (bytes_received == 0) {
                        closesocket(*m_connected_service_socket);
                        m_connected_service_socket = nullptr;
                        LOG_INFO("QUEUEING_SERVICE_SERVER_RECV_FROM_CONNECTED_SERVICE", "Queueing service disconnected.", WSAGetLastError());
                        delete[] recv_buffer;
                        break;
                    }

                    handle_recv_from_connected_service(recv_buffer, bytes_received);
                    break;
                };
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    void queueing_service_server::handle_recv_from_connected_service(char* t_buffer, unsigned int t_length) {
        // total len of all messages received
        unsigned int to_process = t_length;
        // pointer to begining of buffer
        char* pos = t_buffer;

        common::binary_reader binary_reader{};

        // keep going untill you process all messages
        while (to_process != 0) {
            // get message type
            common::message_type type = (common::message_type)(ntohs(binary_reader.read_short(pos)));
            to_process -= sizeof(short);

            // get data length
            short len = ntohs(binary_reader.read_short(pos));
            to_process -= sizeof(short);

            switch (type) {
                case common::message_type::t_int: {
                    // get message data
                    int* data = new int(ntohl(binary_reader.read_int(pos)));
                    to_process -= sizeof(int);

                    message<int> msg(receiver::client, data);
                    m_parent->m_queue_int.insert_back(msg);

                    LOG_INFO("QUEUEING_SERVICE_SERVER_HANDLE_RECV_INT", "Message received from service: %d", *data);
                    break;
                }
                case common::message_type::t_float: {
                    // get message data
                    float* data = new float(ntohf(binary_reader.read_float(pos)));
                    to_process -= sizeof(float);

                    message<float> msg(receiver::client, data);
                    m_parent->m_queue_float.insert_back(msg);

                    LOG_INFO("QUEUEING_SERVICE_SERVER_HANDLE_RECV_FLOAT", "Message received from service: %.2f", *data);
                    break;
                }
                case common::message_type::t_double: {
                    // get message data
                    double* data = new double(ntohd(binary_reader.read_double(pos)));
                    to_process -= sizeof(double);

                    message<double> msg(receiver::client, data);
                    m_parent->m_queue_double.insert_back(msg);

                    LOG_INFO("QUEUEING_SERVICE_SERVER_RECV_DOUBLE", "Message received from service: %.2f", *data);
                    break;
                }
                case common::message_type::t_short: {
                    // get message data
                    short* data = new short(ntohs(binary_reader.read_short(pos)));
                    to_process -= sizeof(short);

                    message<short> msg(receiver::client, data);
                    m_parent->m_queue_short.insert_back(msg);

                    LOG_INFO("QUEUEING_SERVICE_SERVER_HANDLE_RECV_SHORT", "Message received from service: %d", *data);
                    break;
                }
                case common::message_type::t_char: {
                    // get message data
                    char* data = binary_reader.read_char(pos, len);
                    to_process -= len;

                    message<char> msg(receiver::client, data);
                    m_parent->m_queue_char.insert_back(msg);

                    LOG_INFO("QUEUEING_SERVICE_SERVER_HANDLE_RECV_CHAR", "Message received from service: %s", data);
                    break;
                }
                default: break;
            }
        };

        LOG_INFO("QUEUEING_SERVICE_SERVER_HANDLE_RECV", "Total bytes received from service: %d", t_length);
        delete[] t_buffer;
    }
}