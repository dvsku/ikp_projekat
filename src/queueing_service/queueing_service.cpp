#include "queueing_service.h"
#include "./helpers/message_serializer.h"
#include "./helpers/common_enums.h"
#include "./helpers/logger.h"

namespace queueing_service {
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	CTOR
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	queueing_service::queueing_service(unsigned short t_service_port, unsigned short t_clients_port, bool t_is_host) :
		m_service_port(t_service_port), m_clients_port(t_clients_port), m_is_host(t_is_host), 
		m_clients_server(this), m_service_server(this), m_service_client(this),
		m_queue_int("int"), m_queue_float("float"), m_queue_double("double"), m_queue_short("short"), m_queue_char("char") {

		m_stop = false;
		m_process_queue_int = std::thread(&queueing_service::process_int, this);
		m_process_queue_float = std::thread(&queueing_service::process_float, this);
		m_process_queue_double = std::thread(&queueing_service::process_double, this);
		m_process_queue_short = std::thread(&queueing_service::process_short, this);
		m_process_queue_char = std::thread(&queueing_service::process_char, this);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	CONNECTION METHODS
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/// <summary>
	/// Start the queueing service as a host for another queueing service.
	/// </summary>
	/// <returns>-1 if an error occurred, 0 if it was successful.</returns>
	int queueing_service::start_as_host() {
		if (m_service_server.start(m_service_port) == -1 || m_clients_server.start(m_clients_port) == -1) {
			return -1;
		}
		
		LOG_INFO("QS", "Accepting service connection on port %d.", m_service_port);
		LOG_INFO("QS", "Accepting client connections on port %d.", m_clients_port);
		return 0;
	}

	/// <summary>
	/// Starts the queueing service as a client and connects to the host.
	/// </summary>
	/// <returns>-1 if an error occurred, 0 if it was successful.</returns>
	int queueing_service::start_as_client() {
		if (m_service_client.establish_connection(m_service_port) == -1 || m_clients_server.start(m_clients_port) == -1) {
			return -1;
		}
		LOG_INFO("QS", "Connected to service on port %d.", m_service_port);
		LOG_INFO("QS", "Accepting client connections on port %d.", m_clients_port);
		return 0;
	}

	/// <summary>
	/// Stops the queueing service that was started as a host.
	/// </summary>
	void queueing_service::stop_host() {
		m_stop = true;
		m_service_server.stop();
		m_clients_server.stop();
		clean_queues();
	}

	/// <summary>
	/// Stops the queueing service that was started as a client.
	/// </summary>
	void queueing_service::stop_client() {
		m_stop = true;
		m_service_client.disconnect();
		m_clients_server.stop();
		clean_queues();
	}

	/// <summary>
	/// Cleanup when a client disconnects.
	/// </summary>
	/// <param name="t_client_socket">Client socket.</param>
	void queueing_service::queueing_service::on_client_disconnected(SOCKET t_client_socket) {
		disconnect_from_queue(t_client_socket);
		m_clients.remove_item_by_value(t_client_socket);
		closesocket(t_client_socket);
	}

	/// <summary>
	/// Cleanup when a service disconnects.
	/// </summary>
	void queueing_service::queueing_service::on_service_disconnected() {
		m_queue_int.m_client_connected = false;
		m_queue_float.m_client_connected = false;
		m_queue_double.m_client_connected = false;
		m_queue_short.m_client_connected = false;
		m_queue_char.m_client_connected = false;
	}

	/// <summary>
	/// Send a message to the other service.
	/// </summary>
	/// <param name="t_msg">Message</param>
	/// <param name="t_len">Message length</param>
	/// <returns>-1 if an error occurred, bytes sent if it was successful.</returns>
	int queueing_service::send_message_to_service(char* t_msg, unsigned int t_len) {
		if (m_is_host)
			return m_service_server.send_message(t_msg, t_len);
		else
			return m_service_client.send_message(t_msg, t_len);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	QUEUE METHODS
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/// <summary>
	/// Check if there is a client connected to a queue by a specified name.
	/// </summary>
	/// <param name="t_name">Queue name</param>
	/// <returns>true if there is a queue by that name and a client connected to it, otherwise false.</returns>
	bool queueing_service::queueing_service::is_client_connected_to_queue(std::string t_name) {
		return m_queue_int.is_client_connected_to_queue(t_name) || m_queue_float.is_client_connected_to_queue(t_name) ||
			m_queue_double.is_client_connected_to_queue(t_name) || m_queue_short.is_client_connected_to_queue(t_name) ||
			m_queue_char.is_client_connected_to_queue(t_name);
	}
	
	/// <summary>
	/// Tries to connect a client to a queue by a specified name and notify the other service that a client has
	/// connected to the queue.
	/// </summary>
	/// <param name="t_name">Queue name</param>
	/// <param name="t_client_socket">Client socket</param>
	/// <returns>true if connection was successful, otherwise false.</returns>
	bool queueing_service::queueing_service::connect_to_queue(std::string t_name, SOCKET t_client_socket) {
		if (m_queue_int.m_name == t_name && m_queue_int.connect_to_queue(t_client_socket)) {
			notify_connected_to_queue(common::queue_type::t_int);
			return true;
		}
		else if (m_queue_float.m_name == t_name && m_queue_float.connect_to_queue(t_client_socket)) {
			notify_connected_to_queue(common::queue_type::t_float);
			return true;
		}
		else if (m_queue_double.m_name == t_name && m_queue_double.connect_to_queue(t_client_socket)) {
			notify_connected_to_queue(common::queue_type::t_double);
			return true;
		}
		else if (m_queue_short.m_name == t_name && m_queue_short.connect_to_queue(t_client_socket)) {
			notify_connected_to_queue(common::queue_type::t_short);
			return true;
		}
		else if (m_queue_char.m_name == t_name && m_queue_char.connect_to_queue(t_client_socket)) {
			notify_connected_to_queue(common::queue_type::t_char);
			return true;
		}
			
		return false;
	}

	/// <summary>
	/// Disconect a client from a queue and notify the other service that the client has disconnected from the queue.
	/// </summary>
	/// <param name="t_client_socket">Client socket</param>
	void queueing_service::queueing_service::disconnect_from_queue(SOCKET t_client_socket) {
		if (m_queue_int.disconnect_from_queue(t_client_socket))
			notify_disconnected_from_queue(common::queue_type::t_int);
		else if (m_queue_float.disconnect_from_queue(t_client_socket))
			notify_disconnected_from_queue(common::queue_type::t_float);
		else if (m_queue_double.disconnect_from_queue(t_client_socket))
			notify_disconnected_from_queue(common::queue_type::t_double);
		else if (m_queue_short.disconnect_from_queue(t_client_socket))
			notify_disconnected_from_queue(common::queue_type::t_short);
		else if (m_queue_char.disconnect_from_queue(t_client_socket))
			notify_disconnected_from_queue(common::queue_type::t_char);
	}

	/// <summary>
	/// Send a message to the other service that a client has connected to a specific queue.
	/// </summary>
	/// <param name="t_queue_type">Queue type</param>
	/// <returns>-1 if an error occurred, bytes sent if it was successful.</returns>
	int queueing_service::queueing_service::notify_connected_to_queue(common::queue_type t_queue_type) {
		unsigned int total_size;
		char* message = serialize_message(common::message_type::t_short, common::command::stos_client_connected_to_queue, &t_queue_type, &total_size);

		int result = send_message_to_service(message, total_size);
		delete[] message;
		return result;
	}

	/// <summary>
	/// Send a message to the other service that a client has disconnected from a specific queue.
	/// </summary>
	/// <param name="t_queue_type">Queue type</param>
	/// <returns>-1 if an error occurred, bytes sent if it was successful.</returns>
	int queueing_service::queueing_service::notify_disconnected_from_queue(common::queue_type t_queue_type) {
		unsigned int total_size;
		char* message = serialize_message(common::message_type::t_short, common::command::stos_client_disconnected_from_queue, &t_queue_type, &total_size);

		int result = send_message_to_service(message, total_size);
		delete[] message;
		return result;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	MESSAGE PROCESSING METHODS
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/// <summary>
	/// Process int queue messages.
	/// </summary>
	void queueing_service::process_int() {
		while (!m_stop) {
			if (m_queue_int.is_empty()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;
			}

			message<int> msg = m_queue_int.take_first_element();
			unsigned int size = 0;
			// send
			if (msg.m_receiver == receiver::client) {
				char* message = serialize_message(common::message_type::t_int, common::command::stoc_send_message, msg.m_data, &size);

				SOCKET client_socket = m_queue_int.get_connected_client();
				if (client_socket == NULL) {
					LOG_WARNING("QS_PROCESS_INT", "There was no client connected to queue, deleting message.");
				}
				else {
					int result = send(client_socket, message, size, 0);
					if (result == SOCKET_ERROR) {
						LOG_ERROR("QS_PROCESS_INT", "Send message to client failed, deleting message.");
						// close socket
					}
					else {
						LOG_INFO("QS_PROCESS_INT", "Message sent to client. Total bytes sent: %d.", result);
					}
				}
				delete msg.m_data;
				delete[] message;
			}
			else {
				char* message = serialize_message(common::message_type::t_int, common::command::stos_send_message, msg.m_data, &size);

				int result = send_message_to_service(message, size);
				if (result == -1) {		// message was not sent
					m_queue_int.insert_back(msg);
				}
				else {					// message was sent
					LOG_INFO("QS_PROCESS_INT", "Message sent to service. Total bytes sent: %d.", result);
					delete msg.m_data;
				}
				delete[] message;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}

	/// <summary>
	/// Process float queue messages.
	/// </summary>
	void queueing_service::process_float() {
		while (!m_stop) {
			if (m_queue_float.is_empty()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;
			}

			message<float> msg = m_queue_float.take_first_element();
			unsigned int size = 0;
			// send
			if (msg.m_receiver == receiver::client) {
				char* message = serialize_message(common::message_type::t_float, common::command::stoc_send_message, msg.m_data, &size);

				SOCKET client_socket = m_queue_float.get_connected_client();
				if (client_socket == NULL) {
					LOG_WARNING("QS_PROCESS_FLOAT", "There was no client connected to queue, deleting message.");
				}
				else {
					int result = send(client_socket, message, size, 0);
					if (result == SOCKET_ERROR) {
						LOG_ERROR("QS_PROCESS_FLOAT", "Send message to client failed, deleting message.");
					}
					else {
						LOG_INFO("QS_PROCESS_FLOAT", "Message sent to client. Total bytes sent: %d.", result);
					}
				}
				delete msg.m_data;
				delete[] message;
			}
			else {
				char* message = serialize_message(common::message_type::t_float, common::command::stos_send_message, msg.m_data, &size);

				int result = send_message_to_service(message, size);
				if (result == -1) {		// message was not sent
					m_queue_float.insert_back(msg);
				}
				else {					// message was sent
					LOG_INFO("QS_PROCESS_FLOAT", "Message sent to service. Total bytes sent: %d.", result);
					delete msg.m_data;
				}

				delete[] message;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}

	/// <summary>
	/// Process double queue messages.
	/// </summary>
	void queueing_service::process_double() {
		while (!m_stop) {
			if (m_queue_double.is_empty()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;
			}

			message<double> msg = m_queue_double.take_first_element();
			unsigned int size = 0;
			// send
			if (msg.m_receiver == receiver::client) {
				char* message = serialize_message(common::message_type::t_double, common::command::stoc_send_message, msg.m_data, &size);

				SOCKET client_socket = m_queue_double.get_connected_client();
				if (client_socket == NULL) {
					LOG_WARNING("QS_PROCESS_DOUBLE", "There was no client connected to queue, deleting message.");
				}
				else {
					int result = send(client_socket, message, size, 0);
					if (result == SOCKET_ERROR) {
						LOG_ERROR("QS_PROCESS_DOUBLE", "Send message to client failed, deleting message.");
					}
					else {
						LOG_INFO("QS_PROCESS_DOUBLE", "Message sent to client. Total bytes sent: %d.", result);
					}
				}
				delete msg.m_data;
				delete[] message;
			}
			else {
				char* message = serialize_message(common::message_type::t_double, common::command::stos_send_message, msg.m_data, &size);

				int result = send_message_to_service(message, size);
				if (result == -1) {		// message was not sent
					m_queue_double.insert_back(msg);
				}
				else {					// message was sent
					LOG_INFO("QS_PROCESS_DOUBLE", "Message sent to service. Total bytes sent: %d.", result);
					delete msg.m_data;
				}

				delete[] message;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}

	/// <summary>
	/// Process short queue messages.
	/// </summary>
	void queueing_service::process_short() {
		while (!m_stop) {
			if (m_queue_short.is_empty()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;
			}

			message<short> msg = m_queue_short.take_first_element();
			unsigned int size = 0;
			// send
			if (msg.m_receiver == receiver::client) {
				char* message = serialize_message(common::message_type::t_short, common::command::stoc_send_message, msg.m_data, &size);

				SOCKET client_socket = m_queue_short.get_connected_client();
				if (client_socket == NULL) {
					LOG_WARNING("QS_PROCESS_SHORT", "There was no client connected to queue, deleting message.");
				}
				else {
					int result = send(client_socket, message, size, 0);
					if (result == SOCKET_ERROR) {
						LOG_ERROR("QS_PROCESS_SHORT", "Send message to client failed, deleting message.");
					}
					else {
						LOG_INFO("QS_PROCESS_SHORT", "Message sent to client. Total bytes sent: %d.", result);
					}
				}
				delete msg.m_data;
				delete[] message;
			}
			else {
				char* message = serialize_message(common::message_type::t_short, common::command::stos_send_message, msg.m_data, &size);

				int result = send_message_to_service(message, size);
				if (result == -1) {		// message was not sent
					m_queue_short.insert_back(msg);
				}
				else {					// message was sent
					LOG_INFO("QS_PROCESS_SHORT", "Message sent to service. Total bytes sent: %d.", result);
					delete msg.m_data;
				}

				delete[] message;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}

	/// <summary>
	/// Process char queue messages.
	/// </summary>
	void queueing_service::process_char() {
		while (!m_stop) {
			if (m_queue_char.is_empty()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				continue;
			}

			message<char> msg = m_queue_char.take_first_element();
			unsigned int size = 0;
			// send
			if (msg.m_receiver == receiver::client) {
				char* message = serialize_message(common::message_type::t_char, common::command::stoc_send_message, msg.m_data, &size);

				SOCKET client_socket = m_queue_char.get_connected_client();
				if (client_socket == NULL) {
					LOG_WARNING("QS_PROCESS_CHAR", "There was no client connected to queue, deleting message.");
				}
				else {
					int result = send(client_socket, message, size, 0);
					if (result == SOCKET_ERROR) {
						LOG_ERROR("QS_PROCESS_CHAR", "Send message to client failed, deleting message.");
					}
					else {
						LOG_INFO("QS_PROCESS_CHAR", "Message sent to client. Total bytes sent: %d.", result);
					}
				}
				delete[] msg.m_data;
				delete[] message;
			}
			else {
				char* message = serialize_message(common::message_type::t_char, common::command::stos_send_message, msg.m_data, &size);

				int result = send_message_to_service(message, size);
				if (result == -1) {		// message was not sent
					m_queue_char.insert_back(msg);
				}
				else {					// message was sent
					LOG_INFO("QS_PROCESS_CHAR", "Message sent to service. Total bytes sent: %d.", result);
					delete[] msg.m_data;
				}
				delete[] message;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	CLEANUP METHODS
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/// <summary>
	/// Stops threads processing the queues and frees memory allocated by messages.
	/// </summary>
	void queueing_service::clean_queues() {
		if (m_process_queue_int.joinable())
			m_process_queue_int.join();

		if (m_process_queue_float.joinable())
			m_process_queue_float.join();

		if (m_process_queue_double.joinable())
			m_process_queue_double.join();

		if (m_process_queue_short.joinable())
			m_process_queue_short.join();

		if (m_process_queue_char.joinable())
			m_process_queue_char.join();

		unsigned int cnt = m_queue_int.count();
		for (unsigned int i = 0; i < cnt; i++) {
			message<int> msg = m_queue_int.take_first_element();
			delete msg.m_data;
		}

		cnt = m_queue_float.count();
		for (unsigned int i = 0; i < cnt; i++) {
			message<float> msg = m_queue_float.take_first_element();
			delete msg.m_data;
		}

		cnt = m_queue_double.count();
		for (unsigned int i = 0; i < cnt; i++) {
			message<double> msg = m_queue_double.take_first_element();
			delete msg.m_data;
		}

		cnt = m_queue_short.count();
		for (unsigned int i = 0; i < cnt; i++) {
			message<short> msg = m_queue_short.take_first_element();
			delete msg.m_data;
		}

		cnt = m_queue_char.count();
		for (unsigned int i = 0; i < cnt; i++) {
			message<char> msg = m_queue_char.take_first_element();
			delete[] msg.m_data;
		}
	}
}