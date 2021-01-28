#include "process_client.h"
#include "./helpers/logger.h"
#include "./helpers/binary_reader.h"
#include "./helpers/common_enums.h"

process_client::process_client() : base_client::base_client() {

}

void process_client::handle_recv(char* buffer, unsigned int length) {
    // total len of all messages received
    unsigned int to_process = length;
    // pointer to begining of buffer
    char* pos = buffer;

    common::binary_reader binary_reader{};

    while (to_process != 0) {
        // get message type
        common::message_type type = (common::message_type)(ntohs(binary_reader.read_short(pos)));
        to_process -= sizeof(short);

        // get command
        common::command command = (common::command)(ntohs(binary_reader.read_short(pos)));
        to_process -= sizeof(short);

        // get data length
        short len = ntohs(binary_reader.read_short(pos));
        to_process -= sizeof(short);

        if (command == common::command::stoc_queue_client_exists) {
            bool exists = binary_reader.read_bool(pos);
            to_process -= sizeof(bool);

            if(exists)
                printf("Exists: true\n");
            else
                printf("Exists: false\n");
        }
        else if (command == common::command::stoc_connect_to_queue) {
            bool connected = binary_reader.read_bool(pos);
            to_process -= sizeof(bool);

            if (connected)
                printf("Connected: true\n");
            else
                printf("Connected: false\n");
        }
        else if (command == common::command::stoc_send_message) {
            char* data = binary_reader.read_char(pos, len);
            to_process -= len;

            printf("Received message: %s\n", data);
            delete[] data;
        }
    }

	//LOG_INFO("PC_RECV", "Total bytes received from service: %d", length);
	delete[] buffer;
}

void process_client::handle_disconnect() {

}