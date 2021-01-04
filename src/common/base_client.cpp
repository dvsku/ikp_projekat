#include <stdio.h>
#include <conio.h>

#include "base_client.h"
#include "./helpers/logger.h"

#define SERVER_IP_ADDERESS "127.0.0.1"
#define BUFFER_SIZE 1024

namespace common {
	base_client::base_client() {
		m_client_socket = NULL;
	}

	/// <summary>
	/// Connect to a host.
	/// </summary>
	/// <param name="port">Port</param>
	/// <returns>-1 if an error occurred, 0 if it was successful.</returns>
	int base_client::establish_connection(int t_port) {
		sockaddr_in serverAddress{};
		int sockAddrLen = sizeof(struct sockaddr);
		WSADATA wsaData;

		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0) {
			LOG_ERROR("BC_ESTABLISH_CONNECTION", "WSAStartup failed with error: %d", result);
			return -1;
		}

		memset((char*)&serverAddress, 0, sizeof(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDERESS);
		serverAddress.sin_port = htons((u_short)t_port);

		this->m_client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (this->m_client_socket == INVALID_SOCKET) {
			LOG_ERROR("BC_ESTABLISH_CONNECTION", "Creating socket failed with error: %d", WSAGetLastError());
			return -1;
		}

		if (connect(this->m_client_socket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
			LOG_ERROR("BC_ESTABLISH_CONNECTION", "connect failed with error: %d", WSAGetLastError());
			closesocket(this->m_client_socket);
			return -1;
		}

		m_socket_created = true;
		m_t_recv = std::thread(&base_client::do_recv, this);
		return 0;
	}

	/// <summary>
	/// Disconnect from the host.
	/// </summary>
	void base_client::disconnect() {
		m_stop = true;

		if (m_t_recv.joinable())
			m_t_recv.join();

		if (this->m_client_socket != NULL) {
			if (closesocket(this->m_client_socket) == SOCKET_ERROR) {
				LOG_ERROR("BC_DISCONNECT", "closesocket failed with error: %d", WSAGetLastError());
				return;
			}
		}

		m_socket_created = false;
	}

	/// <summary>
	/// Send a message to the host.
	/// </summary>
	/// <param name="msg">Message</param>
	/// <param name="len">Message length</param>
	/// <returns>-1 if an error occured, bytes sent if it was successful.</returns>
	int base_client::send_message(char* t_buffer, unsigned int t_length) {
		if (m_client_socket != NULL && m_socket_created) {
			int ret_val = send(m_client_socket, t_buffer, t_length, 0);
			if (ret_val == SOCKET_ERROR) {
				LOG_ERROR("BC_SEND_MESSAGE", "send failed with error: %d", WSAGetLastError());
				return ret_val;
			}
			else {
				return ret_val;
			}
		}
		return -1;
	}

	/// <summary>
	/// Receive messages from the host.
	/// </summary>
	void base_client::do_recv() {
		struct timeval timeout {};
		struct fd_set fds {};

		timeout.tv_sec = 0;
		timeout.tv_usec = 50;

		while (!m_stop) {
			if (m_client_socket == NULL) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;
			}

			FD_ZERO(&fds);
			FD_SET(m_client_socket, &fds);

			switch (select(0, &fds, 0, 0, &timeout)) {
				case 0: {  // timeout
					break;
				};
				case -1: {  // error
					LOG_ERROR("BC_RECV", "select failed with error: %d", WSAGetLastError());
					break;
				};
				default: {  // success
					char* recv_buffer = new char[BUFFER_SIZE];

					int bytes_received = recv(m_client_socket, recv_buffer, BUFFER_SIZE, 0);

					if (bytes_received == SOCKET_ERROR) {
						closesocket(m_client_socket);
						m_client_socket = NULL;
						delete[] recv_buffer;
						break;
					}

					if (bytes_received == 0) {
						closesocket(m_client_socket);
						m_client_socket = NULL;
						delete[] recv_buffer;
						break;
					}


					handle_recv(recv_buffer, bytes_received);
					break;
				};
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
}