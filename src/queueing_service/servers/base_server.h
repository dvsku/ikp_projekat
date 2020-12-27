#pragma once
#pragma comment(lib,"WS2_32")

#ifndef QUEUEING_SERVICE_SERVERS_BASE_SERVER_H
#define QUEUEING_SERVICE_SERVERS_BASE_SERVER_H

#include <winsock2.h>

#include <thread>

namespace queueing_service {
	class base_server {
		private:
			SOCKET m_listening_socket;
			std::thread m_process_accept;

		protected:
			bool m_stop = false;

		public:
			base_server();
			int start(unsigned short t_port);
			void stop();

		private:
			void accept_connections();

		protected:
			virtual void handle_accept(SOCKET* t_socket) = 0;
	};
}

#endif