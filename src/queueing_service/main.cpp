#include <conio.h>

#include <string>

#include "queueing_service.h"
#include "./helpers/logger.h"

void print_usage();

int main(int argc, char* argv[]) {		
	if (argc < 5 || argc > 5) {
		print_usage();
		return 0;
	}
	if ((strcmp(argv[1], "-s") != 0 && strcmp(argv[1], "-c") != 0) || strcmp(argv[3], "-p") != 0) {
		print_usage();
		return 0;
	}

	bool start_as_host = strcmp(argv[1], "-s") == 0;
	int service_port = atoi(argv[2]);
	int clients_port = atoi(argv[4]);

	queueing_service::queueing_service* p_q_service = new queueing_service::queueing_service(service_port, clients_port, start_as_host);
	
	if (start_as_host) {
		if (p_q_service->start_as_host() == -1) {
			LOG_INFO("MAIN", "Failed to start as host. Exiting.");
			p_q_service->stop_host();
			delete p_q_service;
			return 1;
		}
	}
	else {
		if (p_q_service->start_as_client() == -1) {
			LOG_INFO("MAIN", "Failed to start as client. Exiting.");
			p_q_service->stop_client();
			delete p_q_service;
			return 2;
		}
	}

	while (true) {
		std::string operation;
		std::getline(std::cin, operation);

		if (operation == "exit") break;
	}

	if (start_as_host)
		p_q_service->stop_host();
	else
		p_q_service->stop_client();

	delete p_q_service;
	
	WSACleanup();
	return 0;
}

void print_usage() {
	printf("Usage:\n");
	printf("queueing_service.exe -s|-c <service_port> -p <clients_port>\n");
	printf("\t-s\t\t\t run service as host\n");
	printf("\t-c\t\t\t run service as client\n");
	printf("\t<service_port>\t\t port for service client to connect or service host to open\n");
	printf("\t-p\t\t\t specifies that next argument is port for clients to connect\n");
	printf("\t<clients_port>\t\t port for clients to connect\n");
}