#pragma once

#ifndef QUEUEING_SERVICE_SERVERS_PROCESS_SERVER_H
#define QUEUEING_SERVICE_SERVERS_PROCESS_SERVER_H

#include <thread>

#include "base_server.h"
#include "./helpers/thread_pool.h"

#define PROCESS_RECV_THREADS 3

namespace queueing_service {
	class queueing_service;

	class process_server : public base_server {
		private:
			std::thread m_clients_recv;
			common::thread_pool m_clients_process_recv;
			queueing_service* m_parent;

		public:
			process_server(queueing_service* t_parent);
			void handle_accept(SOCKET t_socket);
			int start(unsigned int t_port);
			void stop();

		private:
			void recv_from_clients();
			void handle_recv(SOCKET t_socket);
	};
}

#endif