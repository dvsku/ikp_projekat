#include "pch.h"
#include "client.h"
#include <stdio.h>
#include <conio.h>

#define SERVER_IP_ADDERESS "127.0.0.1"
#define OUTGOING_BUFFER_SIZE 1024

client::client() {
	m_client_socket = NULL;
}

void client::connect_to_service(int port) {
	sockaddr_in serverAddress;
	int sockAddrLen = sizeof(struct sockaddr);
	char outgoingBuffer[OUTGOING_BUFFER_SIZE];
	int iResult;
	WSADATA wsaData;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		wprintf(L"WSAStartup failed with error: %d\n", iResult);
		return;
	}

	memset((char*)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDERESS);
	serverAddress.sin_port = htons((u_short)port);

	this->m_client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (this->m_client_socket == INVALID_SOCKET) {
		wprintf(L"creating socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	if (connect(this->m_client_socket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
		wprintf(L"connect failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		closesocket(this->m_client_socket);
		return;
	}

	//printf("Enter message from server:\n");

	//// Read string from user into outgoing buffer
	//gets_s(outgoingBuffer, OUTGOING_BUFFER_SIZE);

	//iResult = sendto(this->m_client_socket, outgoingBuffer, strlen(outgoingBuffer), 0, (LPSOCKADDR)&serverAddress, sockAddrLen);

	//if (iResult == SOCKET_ERROR) {
	//	printf("sendto failed with error: %d\n", WSAGetLastError());
	//	closesocket(this->m_client_socket);
	//	WSACleanup();
	//	return 1;
	//}

	//printf("Message sent to server, press any key to exit.\n");
	//_getch();

	//if (closesocket(this->m_client_socket) == SOCKET_ERROR) {
	//	printf("closesocket failed with error: %d\n", WSAGetLastError());
	//	return 1;
	//}

	//if (WSACleanup() == SOCKET_ERROR) {
	//	printf("WSACleanup failed with error: %ld\n", WSAGetLastError());
	//	return 1;
	//}
}

void client::disconnect_from_service() {
	if (this->m_client_socket != NULL) {
		if (closesocket(this->m_client_socket) == SOCKET_ERROR) {
			printf("closesocket failed with error: %d\n", WSAGetLastError());
			return;
		}

		if (WSACleanup() == SOCKET_ERROR) {
			printf("WSACleanup failed with error: %ld\n", WSAGetLastError());
			return ;
		}
	}
}