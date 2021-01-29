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

bool get_message_type(std::string input, common::message_type* type) {
	if (input == "int") {
		*type = common::message_type::t_int;
		return true;
	}
	else if (input == "float") {
		*type = common::message_type::t_float;
		return true;
	}
	else if (input == "double") {
		*type = common::message_type::t_double;
		return true;
	}
	else if (input == "short") {
		*type = common::message_type::t_short;
		return true;
	}
	else if (input == "char") {
		*type = common::message_type::t_char;
		return true;
	}
	return false;
}

int main(int argc, char* argv[]) {
	if (argc < 2 || argc > 2) {
		return 0;
	}

	int port = atoi(argv[1]);

	process_client client;
	client.establish_connection(port);

	common::message_type current_type;

	while (true) {
		std::string operation = my_getline();

		if (operation == "exit") break;
		if (operation.substr(0, 7) == "connect") {
			std::string queue = operation.substr(8);

			if (!get_message_type(queue, &current_type)) {
				printf("Invalid queue name. \n");
				continue;
			}

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
			std::string message = operation.substr(4);

			char* message_char = new char[message.length() + 1];
			strcpy_s(message_char, message.length() + 1, message.c_str());

			switch (current_type) {
				case common::message_type::t_int : {
					unsigned int total_size;
					int data = atoi(message_char);
					char* msg = serialize_message(current_type, common::command::ctos_send_message, &data, &total_size);

					client.send_message(msg, total_size);
					delete[] msg;
					break;
				}
				case common::message_type::t_float: {
					unsigned int total_size;
					float data = atof(message_char);
					char* msg = serialize_message(current_type, common::command::ctos_send_message, &data, &total_size);

					client.send_message(msg, total_size);
					delete[] msg;
					break;
				}
				case common::message_type::t_double: {
					unsigned int total_size;
					double data = atof(message_char);
					char* msg = serialize_message(current_type, common::command::ctos_send_message, &data, &total_size);

					client.send_message(msg, total_size);
					delete[] msg;
					break;
				}
				case common::message_type::t_short: {
					unsigned int total_size;
					short data = (short)atoi(message_char);
					char* msg = serialize_message(current_type, common::command::ctos_send_message, &data, &total_size);

					client.send_message(msg, total_size);
					delete[] msg;
					break;
				}
				case common::message_type::t_char: {
					unsigned int total_size;
					char* msg = serialize_message(current_type, common::command::ctos_send_message, message_char, &total_size);

					client.send_message(msg, total_size);
					delete[] msg;
					break;
				}
			}

			delete[] message_char;
		}
	}

	client.disconnect();
}