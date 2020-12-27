

namespace common {
    template<typename T> linked_list<T>::linked_list() {
        m_length = 0;
        m_head = nullptr;
        m_tail = nullptr;
    }

    template<typename T> linked_list<T>::~linked_list() {
        m_mutex.lock();
        Node<T>* next = m_head;
        Node<T>* curr = nullptr;
        while (next != nullptr) {
            curr = next;
            next = next->next;
            delete curr;
        }
        m_mutex.unlock();
    }

    template<typename T> bool linked_list<T>::is_empty() {
        m_mutex.lock();
        auto ret_val = p_is_empty();
        m_mutex.unlock();
        return ret_val;
    }

    template<typename T> unsigned int linked_list<T>::length() {
        m_mutex.lock();
        auto ret_val = p_length();
        m_mutex.unlock();
        return ret_val;
    }

    template<typename T> void linked_list<T>::prepend_node(T value) {
        m_mutex.lock();
        p_prepend_node(value);
        m_mutex.unlock();
    }

    template<typename T> void linked_list<T>::append_node(T value) {
        m_mutex.lock();
        p_append_node(value);
        m_mutex.unlock();
    }

    template<typename T> T linked_list<T>::take_first_item() {
        m_mutex.lock();
        auto ret_val = p_take_first_item();
        m_mutex.unlock();
        return ret_val;
    }

    template<typename T> void linked_list<T>::remove_item_at_index(unsigned int index) {
        m_mutex.lock();
        p_remove_item_at_index(index);
        m_mutex.unlock();
    }

    template<typename T> void linked_list<T>::remove_item_by_value(T value) {
        m_mutex.lock();
        p_remove_item_by_value(value);
        m_mutex.unlock();
    }

    template<typename T> T* linked_list<T>::get_item_at_index(unsigned int index) {
        m_mutex.lock();
        auto ret_val = p_get_item_at_index(index);
        m_mutex.unlock();
        return ret_val;
    }

    template<typename T> bool linked_list<T>::p_is_empty() {
        return m_head == nullptr;
    }

    template<typename T>  unsigned int linked_list<T>::p_length() {
        return m_length;
    }

    template<typename T> void linked_list<T>::p_prepend_node(T data) {
        Node<T>* node = new Node<T>;
        node->data = data;
        node->next = m_head;
        if (p_is_empty()) {
            m_tail = node;
        }
        m_head = node;
        m_length++;
    }

    template<typename T> void linked_list<T>::p_append_node(T value) {
        Node<T>* node = nullptr;
        if (p_is_empty()) {
            node = new Node<T>;
            m_head = node;
        }
        else {
            node = m_tail;
            node->next = new Node<T>;
            node = node->next;
        }
        node->data = value;
        node->next = nullptr;
        m_tail = node;
        m_length++;
    }

    template<typename T> T linked_list<T>::p_take_first_item() {
        Node<T> node = *m_head;
        T ret_val = node.data;
        p_remove_item_at_index(0);
        return ret_val;
    }

    template<typename T> void linked_list<T>::p_remove_item_at_index(unsigned int index) {
        if ((index < 0) || (index >= m_length)) {
            return;
        }
        else {
            Node<T>* node = m_head;
            if (index == 0) {
                m_head = node->next;
                delete node;
                if (p_is_empty()) {
                    m_tail = nullptr;
                }
            }
            else {
                for (int i = 0; i < index - 1; i++) {
                    node = node->next;
                }
                Node<T>* tmp = node->next;
                node->next = node->next->next;
                delete tmp;
                if (index == m_length - 1) {
                    m_tail = node;
                }
            }
            m_length--;
        }
    }

    template<typename T> void linked_list<T>::p_remove_item_by_value(T value) {
        Node<T>* curr = m_head;
        Node<T>* tmp;

        if (!p_is_empty() && curr->data == value) {
            tmp = curr;
            m_head = curr->next;
            curr = curr->next;
            delete tmp;
            m_length--;
        }
        else {
            while (!p_is_empty() && curr->next != nullptr) {
                if (curr->next->data == value) {
                    tmp = curr->next;
                    curr->next = curr->next->next;
                    delete tmp;
                    m_length--;
                    if (curr->next == nullptr) {
                        m_tail = curr;
                    }
                    break;
                }
                curr = curr->next;
            }
        }
        if (p_is_empty()) {
            m_tail = nullptr;
        }
    }

    template<typename T> T* linked_list<T>::p_get_item_at_index(unsigned int index) {
        if ((index < 0) || (index >= m_length)) {
            return nullptr;
        }
        else {
            Node<T>* node = m_head;
            if (index == 0) {
                return &(node->data);
            }
            else {
                for (int i = 0; i < index - 1; i++) {
                    node = node->next;
                }
                Node<T>* tmp = node->next;
                return &(tmp->data);
            }
        }
    }
}