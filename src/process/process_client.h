#pragma once
#include "base_client.h"

class process_client : public common::base_client {
	public:
		process_client();

	protected:
		void handle_recv(char* buffer, unsigned int length);
		void handle_disconnect();
};