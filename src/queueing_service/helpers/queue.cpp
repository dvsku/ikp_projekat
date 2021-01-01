//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTE: This is not really a .cpp file, it does not compile. It is used to implement queue.h and is included there.
// Hacky way to separate a template class declaration and implementation.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace queueing_service {

	template<class T> queue<T>::queue(std::string name) {
		m_connected_client_socket = NULL;
		m_name = name;
		m_client_connected = false;
	}

	template<class T> bool queue<T>::connect_to_queue(SOCKET t_client_socket) {
		m_mutex.lock();
		auto result = p_connect_to_queue(t_client_socket);
		m_mutex.unlock();
		return result;
	}

	template<class T> bool queue<T>::disconnect_from_queue(SOCKET t_client_socket) {
		m_mutex.lock();
		bool result = false;
		if (t_client_socket == m_connected_client_socket) {
			m_connected_client_socket = NULL;
			result = true;
		}
		m_mutex.unlock();
		return result;
	}

	template<class T> bool queue<T>::is_socket_connected(SOCKET t_client_socket) {
		m_mutex.lock();
		bool result = t_client_socket == m_connected_client_socket;
		m_mutex.unlock();
		return result;
	}

	template<class T> bool queue<T>::is_empty() {
		return m_elements.is_empty();
	}

	template<class T> unsigned int queue<T>::count() {
		return m_elements.length();
	}

	template<class T> void queue<T>::insert_back(T value) {
		m_elements.append_node(value);
	}

	template<class T> T queue<T>::take_first_element() {
		return m_elements.take_first_item();
	}

	template<class T> SOCKET queue<T>::get_connected_client() {
		return m_connected_client_socket;
	}

	template<class T> bool queue<T>::is_client_connected_to_queue(std::string t_name) {
		return m_name == t_name && m_client_connected;
	}

	template<class T> bool queue<T>::p_connect_to_queue(SOCKET t_client_socket) {
		if (m_connected_client_socket != NULL)
			return false;

		m_connected_client_socket = t_client_socket;
		return true;
	}

	/*template<class T> bool queue<T>::p_check_connected_client(SOCKET* t_client_socket)
	{
		return false;
	}*/

}