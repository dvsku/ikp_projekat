#pragma once

#ifndef QUEUEING_SERVICE_QUEUEING_SERVICE_H
#define QUEUEING_SERVICE_QUEUEING_SERVICE_H

#include <thread>
#include <string>

#include "./helpers/queue.h"
#include "./helpers/message.h"
#include "./servers/queueing_service_server.h"
#include "./servers/process_server.h"
#include "./clients/queueing_service_client.h"
#include "linked_list.h"

namespace queueing_service {
	class queueing_service {
		private:
			unsigned short m_service_port;
			unsigned short m_clients_port;
			bool m_is_host;
			bool m_stop{ false };
			std::thread m_process_queue_int;
			std::thread m_process_queue_float;
			std::thread m_process_queue_double;
			std::thread m_process_queue_short;
			std::thread m_process_queue_char;
			queueing_service_server m_service_server;
			queueing_service_client m_service_client;
			process_server m_clients_server;

		public:
			queue<message<int>>	m_queue_int;
			queue<message<float>>	m_queue_float;
			queue<message<double>>	m_queue_double;
			queue<message<short>>	m_queue_short;
			queue<message<char>>	m_queue_char;
			common::linked_list<SOCKET*> m_clients;
			bool queue_exists(std::string t_name);
			bool connect_to_queue(std::string t_name, SOCKET* t_client_socket);
			
			void on_client_disconnected(SOCKET* t_client_socket);

		public:
			queueing_service(unsigned short t_service_port, unsigned short t_clients_port, bool t_is_host);
			int start_as_host();
			int start_as_client();
			void stop_host();
			void stop_client();
			int send_message_to_service(char* t_msg, unsigned int t_len);

		private:
			void process_int();
			void process_float();
			void process_double();
			void process_short();
			void process_char();
			void clean_queues();
			void disconnect_from_queue(SOCKET* t_client_socket);
		};
}

#endif