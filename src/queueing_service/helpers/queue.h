#pragma once

#ifndef QUEUEING_SERVICE_HELPERS_QUEUE_H
#define QUEUEING_SERVICE_HELPERS_QUEUE_H

#include <winsock2.h>

#include <string>
#include <thread>

#include "./helpers/linked_list.h"

#define QUEUE_NAME_LEN 32

namespace queueing_service {
	template<class T> class queue {
		private:
			SOCKET m_connected_client_socket;
			common::linked_list<T> m_elements;
			std::mutex m_mutex;

		public:
			std::string m_name;
			bool m_client_connected;

		private:
			bool p_connect_to_queue(SOCKET t_client_socket);

		public:
			queue<T>(std::string name);
			bool connect_to_queue(SOCKET t_client_socket);
			bool disconnect_from_queue(SOCKET t_client_socket);
			bool is_socket_connected(SOCKET t_client_socket);
			bool is_empty();
			unsigned int count();
			void insert_back(T value);
			T take_first_element();
			SOCKET get_connected_client();
			bool is_client_connected_to_queue(std::string t_name);

	};
}

#include "queue.cpp"

#endif