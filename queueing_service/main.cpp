#include <iostream>
#include "qs_server.h"
#include "qs_client.h"
#include "queue.h"
#include <conio.h>

int main(int argc, char* argv[]) {
	if (argc == 1)
		return 0;

	if (argc == 3) {
		if (strcmp(argv[1], "-s") == 0) {
			int port = atoi(argv[2]);
			qs_server server = qs_server(port);
			server.start();
		}
		if (strcmp(argv[1], "-c") == 0) {
			int port = atoi(argv[2]);
			qs_client c = qs_client();
			c.connect_to_service(port);
		}
	}

	return 0;
}