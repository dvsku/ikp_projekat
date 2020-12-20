#include "qs_server.h"

qs_server::qs_server(int port) : server::server(port) {

}

void qs_server::handle_accept() {
	printf("accept handled\n");
}

void qs_server::handle_select() {
	printf("select handled\n");
}