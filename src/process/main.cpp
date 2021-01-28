#include <iostream>
#include "process_client.h"
#include <string>
#include "./helpers/message_serializer.h"

std::string my_getline()
{
	std::string result;

	while (!std::getline(std::cin, result) || result.empty());

	return result;
}

int main(int argc, char* argv[]) {
	if (argc < 2 || argc > 2) {
		return 0;
	}

	int port = atoi(argv[1]);

	process_client client;
	client.establish_connection(port);

	while (true) {
		std::string operation = my_getline();

		if (operation == "exit") break;
		if (operation.substr(0, 7) == "connect") {
			std::string queue = operation.substr(8);

			char* queue_char = new char[queue.length() + 1];
			strcpy_s(queue_char, queue.length() + 1, queue.c_str());
			
			unsigned int total_size;
			char* msg = serialize_message(common::message_type::t_char, common::command::ctos_connect_to_queue, queue_char, &total_size);
			
			client.send_message(msg, total_size);

			delete[] queue_char;
		}
		if (operation.substr(0, 6) == "exists") {
			std::string queue = operation.substr(7);

			char* queue_char = new char[queue.length() + 1];
			strcpy_s(queue_char, queue.length() + 1, queue.c_str());

			unsigned int total_size;
			char* msg = serialize_message(common::message_type::t_char, common::command::ctos_queue_client_exists, queue_char, &total_size);

			client.send_message(msg, total_size);

			delete[] queue_char;
		}
		if (operation.substr(0, 3) == "msg") {
			std::string queue = operation.substr(4);

			char* queue_char = new char[queue.length() + 1];
			strcpy_s(queue_char, queue.length() + 1, queue.c_str());

			unsigned int total_size;
			char* msg = serialize_message(common::message_type::t_char, common::command::ctos_send_message, queue_char, &total_size);

			client.send_message(msg, total_size);

			delete[] queue_char;
		}
	}

	client.disconnect();
}