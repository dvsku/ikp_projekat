#pragma once

#ifndef QUEUEING_SERVICE_SERVERS_QUEUEING_SERVICE_SERVER_H
#define QUEUEING_SERVICE_SERVERS_QUEUEING_SERVICE_SERVER_H

#include <winsock2.h>

#include <thread>

#include "base_server.h"

namespace queueing_service {
	class queueing_service;

	class queueing_service_server : public base_server {
		private:
			SOCKET* m_connected_service_socket;
			std::thread m_connected_service_recv;
			queueing_service* m_parent = nullptr;

		public:
			queueing_service_server(queueing_service* t_parent);
			int send_message(char* t_msg, unsigned int t_len);
			int start(unsigned short t_port);
			void stop();
			void handle_accept(SOCKET* t_socket);

		private:
			void recv_from_connected_service();
			void handle_recv_from_connected_service(char* t_buffer, unsigned int t_length);
	};
}

#endif