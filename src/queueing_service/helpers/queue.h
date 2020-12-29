#pragma once

#ifndef QUEUEING_SERVICE_HELPERS_QUEUE_H
#define QUEUEING_SERVICE_HELPERS_QUEUE_H

#include <winsock2.h>

#include <string>
#include <thread>

#include "linked_list.h"

#define QUEUE_NAME_LEN 32

namespace queueing_service {
	template<class T> class queue {
		private:
			SOCKET* m_p_connected_client_socket;
			
			common::linked_list<T> m_elements;
			std::mutex m_mutex;

		public:
			std::string m_name;
			bool m_client_connected;

			queue<T>(std::string name) {
				this->m_p_connected_client_socket = nullptr;
				this->m_name = name;
				this->m_client_connected = false;
			}

			~queue<T>() {}

			bool connect_to_queue(SOCKET* t_client_socket) {
				m_mutex.lock();
				auto result = p_connect_to_queue(t_client_socket);
				m_mutex.unlock();
				return result;
			}

			bool disconnect_from_queue(SOCKET* t_client_socket) {
				m_mutex.lock();
				bool result = false;
				if (t_client_socket == m_p_connected_client_socket) {
					m_p_connected_client_socket = nullptr;
					result = true;
				}
				m_mutex.unlock();
				return result;
			}

			bool is_empty() {
				return m_elements.is_empty();
			}

			unsigned int count() {
				return m_elements.length();
			}

			void insert_back(T value) {
				m_elements.append_node(value);
			}

			T take_first_element() {
				return m_elements.take_first_item();
			}

			SOCKET* get_connected_client() {
				return m_p_connected_client_socket;
			}

			bool is_client_connected_to_queue(std::string t_name) {
				return m_name == t_name && m_client_connected;
			}

		private:
			bool p_connect_to_queue(SOCKET* t_client_socket) {
				if (m_p_connected_client_socket == nullptr) 
					return false;

				m_p_connected_client_socket = t_client_socket;
				return true;
			}

			bool p_check_connected_client(SOCKET* t_client_socket) {
				return t_client_socket == m_p_connected_client_socket;
			}
	};
}

#endif