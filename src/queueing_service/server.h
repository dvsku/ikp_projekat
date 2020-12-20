#include <winsock2.h>
#include <thread>

#pragma once
#pragma comment(lib,"WS2_32")

class server {
	private:
		int m_port;
		SOCKET m_listening_socket;
		SOCKET m_connected_service_socket;
		void do_accept();
	public:
		virtual void handle_accept() = 0;
		virtual void handle_select() = 0;
		int start();
		server(int port);
};