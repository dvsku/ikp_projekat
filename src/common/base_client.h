#pragma once
#pragma comment(lib,"WS2_32")

#ifndef COMMON_BASE_CLIENT_H
#define COMMON_BASE_CLIENT_H

#include <winsock2.h>

#include <thread>

namespace common {
	class base_client {
		private:
			SOCKET m_client_socket;
			std::thread m_t_recv;
			bool m_socket_created = false;

		protected:
			bool m_stop = false;

		public:
			base_client();
			int establish_connection(int t_port);
			void disconnect();
			int send_message(char* t_buffer, unsigned int t_length);

		private:
			void do_recv();

		protected:
			virtual void handle_recv(char* t_buffer, unsigned int t_length) = 0;
			virtual void handle_disconnect() = 0;
	};
}

#endif