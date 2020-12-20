#include <winsock2.h>
#include "pch.h"
#include "framework.h"

#pragma once
#pragma comment(lib,"WS2_32")

class client {
	private:
		SOCKET m_client_socket;
	public:
		void connect_to_service(int port);
		void disconnect_from_service();
		client();
};