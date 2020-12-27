#pragma once

#ifndef QUEUEING_SERVICE_CLIENTS_QUEUEING_SERVICE_CLIENT_H
#define QUEUEING_SERVICE_CLIENTS_QUEUEING_SERVICE_CLIENT_H

#include "base_client.h"

namespace queueing_service {
	class queueing_service;

	class queueing_service_client : public common::base_client {
		private:
			queueing_service* m_p_parent = nullptr;

		public:
			queueing_service_client(queueing_service* t_parent);

		protected:
			void handle_recv(char* t_buffer, unsigned int t_length);
	};
}
#endif