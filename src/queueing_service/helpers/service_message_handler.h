#pragma once

#ifndef QUEUEING_SERVICE_HELPERS_SERVICE_MESSAGE_HANDLER_H
#define QUEUEING_SERVICE_HELPERS_SERVICE_MESSAGE_HANDLER_H

namespace queueing_service {
	class queueing_service;

	class service_message_handler {
		private:
			queueing_service* m_parent;

		public:
			service_message_handler(queueing_service* t_parent);

		protected:
			void handle_message(char* t_buffer, unsigned int t_length);

	};
}

#endif