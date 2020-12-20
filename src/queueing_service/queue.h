#include <winsock2.h>
#pragma once

#include "../common/doubly_linked_list.h"
#include <string>
#include <thread>

#define QUEUE_NAME_LEN 32

template<class T>
class queue {
	private:
		std::string m_name;
		SOCKET* m_p_service_socket;
		SOCKET* m_p_connected_client_socket;
		doubly_linked_list<T> m_elements;
		std::mutex m_mutex;
	public:
		queue<T>(std::string name) {
			this->m_p_service_socket = nullptr;
			this->m_p_connected_client_socket = nullptr;
			this->m_name = name;
		}

		~queue<T>() {}

		void start() {
			std::thread t(do_work);
			t.detach();
		}

		void stop() {

		}

		bool is_empty() {
			return m_elements.is_empty();
		}

		unsigned int count() {
			return m_elements.count();
		}

		void insert_back(T value) {
			m_elements.insert_back(value);
		}

		T take_first_element() {
			return m_elements.take_first_element();
		}

	private:
		void do_work() {
			while (true) {
				if (is_empty()) continue;


			}
		}
};