#include "queueing_service_client.h"
#include "../helpers/enums.h"
#include "../helpers/message.h"
#include "../queueing_service.h"
#include "common_enums.h"
#include "logger.h"
#include "binary_reader.h"

namespace queueing_service {
    queueing_service_client::queueing_service_client(queueing_service* t_parent) : common::base_client::base_client() {
        m_p_parent = t_parent;
    }

    void queueing_service_client::handle_recv(char* t_buffer, unsigned int t_length) {
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

                    // add to queue
                    message<int> msg(receiver::client, data);
                    m_p_parent->m_queue_int.insert_back(msg);

                    LOG_INFO("QUEUEING_SERVICE_CLIENT_HANDLE_RECV_INT", "Message received from service: %d", *data);
                    break;
                }
                case common::message_type::t_float: {
                    // get message data
                    float* data = new float(ntohf(binary_reader.read_float(pos)));
                    to_process -= sizeof(float);

                    // add to queue
                    message<float> msg(receiver::client, data);
                    m_p_parent->m_queue_float.insert_back(msg);

                    LOG_INFO("QUEUEING_SERVICE_CLIENT_HANDLE_RECV_FLOAT", "Message received from service: %.2f", *data);
                    break;
                }
                case common::message_type::t_double: {
                    // get message data
                    double* data = new double(ntohd(binary_reader.read_double(pos)));
                    to_process -= sizeof(double);

                    // add to queue
                    message<double> msg(receiver::client, data);
                    m_p_parent->m_queue_double.insert_back(msg);

                    LOG_INFO("QUEUEING_SERVICE_CLIENT_HANDLE_RECV_DOUBLE", "Message received from service: %.2f", *data);
                    break;
                }
                case common::message_type::t_short: {
                    // get message data
                    short* data = new short(ntohs(binary_reader.read_short(pos)));
                    to_process -= sizeof(short);

                    // add to queue
                    message<short> msg(receiver::client, data);
                    m_p_parent->m_queue_short.insert_back(msg);

                    LOG_INFO("QUEUEING_SERVICE_CLIENT_HANDLE_RECV_SHORT", "Message received from service: %d", *data);
                    break;
                }
                case common::message_type::t_char: {
                    // get message data
                    char* data = binary_reader.read_char(pos, len);
                    to_process -= len;

                    // add to queue
                    message<char> msg(receiver::client, data);
                    m_p_parent->m_queue_char.insert_back(msg);

                    LOG_INFO("QUEUEING_SERVICE_CLIENT_HANDLE_RECV_CHAR", "Message received from service: %s", data);
                    break;
                }
                default: break;
            }
        };

        delete[] t_buffer;
    }
}