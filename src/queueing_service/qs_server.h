#pragma once
#include "server.h"


class qs_server : public server {
	private:	

	public:
		void handle_accept();
		void handle_select();
		qs_server(int port);
};