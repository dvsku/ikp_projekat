#include "service_message_handler.h"
#include "queueing_service.h"
#include "common_enums.h"
#include "binary_reader.h"
#include "logger.h"

namespace queueing_service {
	service_message_handler::service_message_handler(queueing_service* t_parent) {
		m_parent = t_parent;
	}

	void service_message_handler::handle_message(char* t_buffer, unsigned int t_length) {
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
            case common::message_type::t_int:
            {
                // get message data
                int* data = new int(ntohl(binary_reader.read_int(pos)));
                to_process -= sizeof(int);

                message<int> msg(receiver::client, data);
                m_parent->m_queue_int.insert_back(msg);

                LOG_INFO("QS_SMH_INT", "Message received from service: %d", *data);
                break;
            }
            case common::message_type::t_float:
            {
                // get message data
                float* data = new float(ntohf(binary_reader.read_float(pos)));
                to_process -= sizeof(float);

                message<float> msg(receiver::client, data);
                m_parent->m_queue_float.insert_back(msg);

                LOG_INFO("QS_SMH_FLOAT", "Message received from service: %.2f", *data);
                break;
            }
            case common::message_type::t_double:
            {
                // get message data
                double* data = new double(ntohd(binary_reader.read_double(pos)));
                to_process -= sizeof(double);

                message<double> msg(receiver::client, data);
                m_parent->m_queue_double.insert_back(msg);

                LOG_INFO("QS_SMH_DOUBLE", "Message received from service: %.2f", *data);
                break;
            }
            case common::message_type::t_short:
            {
                // get message data
                short* data = new short(ntohs(binary_reader.read_short(pos)));
                to_process -= sizeof(short);

                message<short> msg(receiver::client, data);
                m_parent->m_queue_short.insert_back(msg);

                LOG_INFO("QS_SMH_SHORT", "Message received from service: %d", *data);
                break;
            }
            case common::message_type::t_char:
            {
                // get message data
                char* data = binary_reader.read_char(pos, len);
                to_process -= len;

                message<char> msg(receiver::client, data);
                m_parent->m_queue_char.insert_back(msg);

                LOG_INFO("QS_SMH_CHAR", "Message received from service: %s", data);
                break;
            }
            default: break;
            }
        };

        LOG_INFO("QS_SMH", "Total bytes received from service: %d", t_length);
        delete[] t_buffer;
	}
}