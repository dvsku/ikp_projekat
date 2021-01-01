#include "process_server.h"
#include "../queueing_service.h"
#include "./helpers/message_serializer.h"
#include "./helpers/common_enums.h"
#include "./helpers/logger.h"
#include "./helpers/binary_reader.h"

namespace queueing_service {
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //	CTOR
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    process_server::process_server(queueing_service* t_parent) : base_server::base_server(), m_clients_process_recv(PROCESS_RECV_THREADS) {
        m_parent = t_parent;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //	CONNECTION METHODS
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int process_server::start(unsigned int t_port) {
        int result = base_server::start(t_port);

        m_clients_recv = std::thread(&process_server::recv_from_clients, this);
        return result;
    }
    
    void process_server::stop() {
        // close listening socket and stop its thread
        base_server::stop();

        m_stop = true;
       
        if (m_clients_recv.joinable())
            m_clients_recv.join();
    }

    void process_server::recv_from_clients() {
        struct timeval timeout {};
        struct fd_set fds {};

        timeout.tv_sec = 0;
        timeout.tv_usec = 50;

        while (!m_stop) {
            if (m_parent->m_clients.length() == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            SOCKET current_socket = m_parent->m_clients.take_first_item();

            FD_ZERO(&fds);
            FD_SET(current_socket, &fds);

            int result = select(0, &fds, 0, 0, &timeout);

            if (result == -1) {
                LOG_ERROR("PS_RECV", "select failed with error: %d", WSAGetLastError());
                m_parent->m_clients.append_node(current_socket);
            }
            else if (result > 0) {
                m_clients_process_recv.enqueue([&]() {
                    handle_recv(current_socket);
                });
            }
            else {
                m_parent->m_clients.append_node(current_socket);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //	HANDLERS
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void process_server::handle_accept(SOCKET t_socket) {
        struct sockaddr_storage connected_service_addr {};
        int connected_service_addr_len = sizeof(connected_service_addr);

        getpeername(t_socket, (struct sockaddr*)&connected_service_addr, &connected_service_addr_len);

        struct sockaddr_in* s = (struct sockaddr_in*)&connected_service_addr;

        LOG_INFO("PS_HA", "Client connected from %s:%d", inet_ntoa(s->sin_addr), ntohs(s->sin_port));

        m_parent->m_clients.append_node(t_socket);
    }

    void process_server::handle_recv(SOCKET t_socket) {
        char recv_buffer[1024];
        int bytes_received = recv(t_socket, recv_buffer, sizeof(recv_buffer), 0);

        if (bytes_received == SOCKET_ERROR) {
            m_parent->on_client_disconnected(t_socket);
            LOG_INFO("PS_HRECV", "Client disconnected.");
            return;
        }

        if (bytes_received == 0) {
            m_parent->on_client_disconnected(t_socket);
            LOG_INFO("PS_HRECV", "Client disconnected.");
            return;
        }

        // total len of all messages received
        unsigned int to_process = bytes_received;
        // pointer to begining of buffer
        char* pos = recv_buffer;

        common::binary_reader binary_reader{};

        // keep going untill you process all messages
        while (to_process != 0) {
            // get message type
            common::message_type type = (common::message_type)(ntohs(binary_reader.read_short(pos)));
            to_process -= sizeof(short);

            // get request
            common::command command = (common::command)(ntohs(binary_reader.read_short(pos)));
            to_process -= sizeof(short);

            // get data length
            short len = ntohs(binary_reader.read_short(pos));
            to_process -= sizeof(short);

            if (command == common::command::ctos_connect_to_queue) {
                // get message data
                char* data = binary_reader.read_char(pos, len);
                to_process -= len;

                std::string queue_name(data);
                bool connected = m_parent->connect_to_queue(queue_name, t_socket);
                unsigned int total_size;
                char* response = serialize_message(common::message_type::t_bool, common::command::stoc_connect_to_queue, &connected, &total_size);

                int result = send(t_socket, response, total_size, 0);

                if (result == SOCKET_ERROR) {
                    m_parent->on_client_disconnected(t_socket);
                    delete[] data;
                    delete[] response;
                    return;
                }

                delete[] data;
                delete[] response;
            }
            else if (command == common::command::ctos_queue_client_exists) {
                // get message data
                char* data = binary_reader.read_char(pos, len);
                to_process -= len;

                std::string queue_name(data);
                bool client_exists = m_parent->is_client_connected_to_queue(data);
                unsigned int total_size;
                char* response = serialize_message(common::message_type::t_bool, common::command::stoc_queue_client_exists, &client_exists, &total_size);

                int result = send(t_socket, response, total_size, 0);

                if (result == SOCKET_ERROR) {
                    m_parent->on_client_disconnected(t_socket);
                    delete[] data;
                    delete[] response;
                    return;
                }

                delete[] data;
                delete[] response;
            }
            else if (command == common::command::ctos_send_message) {
                if (type == common::message_type::t_int) {
                    if (!m_parent->m_queue_int.is_socket_connected(t_socket)) {
                        to_process -= sizeof(int);
                        continue;
                    }

                    // get message data
                    int* data = new int(ntohl(binary_reader.read_int(pos)));
                    to_process -= sizeof(int);

                    message<int> msg(receiver::service, data);
                    m_parent->m_queue_int.insert_back(msg);

                    LOG_INFO("PS_HRECV_INT", "Message received from client: %d", *data);
                }
                else if (type == common::message_type::t_float) {
                    if (!m_parent->m_queue_float.is_socket_connected(t_socket)) {
                        to_process -= sizeof(float);
                        continue;
                    }

                    // get message data
                    float* data = new float(ntohf(binary_reader.read_float(pos)));
                    to_process -= sizeof(float);

                    message<float> msg(receiver::service, data);
                    m_parent->m_queue_float.insert_back(msg);

                    LOG_INFO("PS_HRECV_FLOAT", "Message received from client: %.2f", *data);
                }
                else if (type == common::message_type::t_double) {
                    if (!m_parent->m_queue_double.is_socket_connected(t_socket)) {
                        to_process -= sizeof(double);
                        continue;
                    }

                    // get message data
                    double* data = new double(ntohd(binary_reader.read_double(pos)));
                    to_process -= sizeof(double);

                    message<double> msg(receiver::service, data);
                    m_parent->m_queue_double.insert_back(msg);

                    LOG_INFO("PS_HRECV_DOUBLE", "Message received from client: %.2f", *data);
                }
                else if (type == common::message_type::t_short) {
                    if (!m_parent->m_queue_short.is_socket_connected(t_socket)) {
                        to_process -= sizeof(short);
                        continue;
                    }

                    // get message data
                    short* data = new short(ntohs(binary_reader.read_short(pos)));
                    to_process -= sizeof(short);

                    message<short> msg(receiver::service, data);
                    m_parent->m_queue_short.insert_back(msg);

                    LOG_INFO("PS_HRECV_SHORT", "Message received from client: %d", *data);
                }
                else if (type == common::message_type::t_char) {
                    if (!m_parent->m_queue_char.is_socket_connected(t_socket)) {
                        to_process -= len;
                        continue;
                    }

                    // get message data
                    char* data = binary_reader.read_char(pos, len);
                    to_process -= len;

                    message<char> msg(receiver::service, data);
                    m_parent->m_queue_char.insert_back(msg);

                    LOG_INFO("PS_HRECV_CHAR", "Message received from client: %s", data);
                }
            }
        }

        m_parent->m_clients.append_node(t_socket);
        LOG_INFO("PS_HRECV", "Total bytes received from client: %d", bytes_received);
    }
}