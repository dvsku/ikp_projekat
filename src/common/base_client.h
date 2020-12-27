#pragma once
#pragma comment(lib,"WS2_32")

#ifndef COMMON_BASE_CLIENT_H
#define COMMON_BASE_CLIENT_H

#include <winsock2.h>

#include <thread>

#include "pch.h"
#include "framework.h"

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
			int establish_connection(int port);
			void disconnect();
			int send_message(char* msg, unsigned int len);

		private:
			void do_recv();

		protected:
			virtual void handle_recv(char* buffer, unsigned int length) = 0;
	};
}

#endif