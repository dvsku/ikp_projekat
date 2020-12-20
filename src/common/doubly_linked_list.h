#include <iostream>
#include <mutex>

template<class T>
class doubly_linked_list {
    public: 
        struct Node {
            T data;
            Node* next;
            Node* prev;
            Node(T val) : data(val), next(nullptr), prev(nullptr) {}
        };

    private:
        Node* m_head;
        Node* m_tail;
        std::mutex m_mutex;

    public:
        doubly_linked_list() {
            m_head = nullptr;
            m_tail = nullptr;
        }

        ~doubly_linked_list() {
            m_mutex.lock();
            Node* tmp = nullptr;
            while (m_head) {
                tmp = m_head;
                m_head = m_head->next;
                delete tmp;
            }
            m_head = nullptr;
            m_mutex.unlock();
        }

        doubly_linked_list(const doubly_linked_list<T>& dll) = delete;
        doubly_linked_list& operator=(doubly_linked_list const&) = delete;

        void insert_front(T value) {
            m_mutex.lock();
            Node* node = new Node(value);
            Node* tmp = m_head;
            if (m_head == nullptr) {
                m_head = node;
                m_tail = node;
            }
            else {
                node->next = m_head;
                m_head = node;
                node->next->prev = node;
            }
            m_mutex.unlock();
        }

        void insert_back(T value) {
            m_mutex.lock();
            Node* node = new Node(value);
            if (m_head == nullptr && m_tail == nullptr) {
                m_head = node;
                m_tail = node;
            }
            else if (m_tail->next == nullptr) {
                m_tail->next = node;
                m_tail = node;
            }
            m_mutex.unlock();
        }

        T take_first_element() {
            Node element = *m_head;
            T ret_val = element.data;
            remove(ret_val);
            return ret_val;
        }

        void remove(T value) {
            Node* to_remove = find(value);
            m_mutex.lock();
            Node* tmp = m_head;

            if (tmp == to_remove) {
                m_head = tmp->next;
            }
            else {
                while (to_remove != nullptr) {
                    if (tmp->next == to_remove) {
                        tmp->next = to_remove->next;
                        to_remove->next->prev = tmp;
                        delete to_remove;
                        m_mutex.unlock();
                        return;
                    }
                    tmp = tmp->next;
                }
            }
            m_mutex.unlock();
        }

        bool is_empty() {
            m_mutex.lock();
            bool ret_val = m_head == nullptr;
            m_mutex.unlock();
            return ret_val;
        }

        unsigned int count() {
            m_mutex.lock();
            unsigned int count = 0;
            Node* node = m_head;
            while (node != nullptr) {
                count++;
                node = node->next;
            }
            m_mutex.unlock();
            return count;
        }

        template <class U>
        friend std::ostream& operator<<(std::ostream& os, const doubly_linked_list<U>& dll) {
            dll.display(os);
            return os;
        }

    private:
        Node* find(T value) {
            m_mutex.lock();
            Node* node = m_head;
            while (node != nullptr) {
                if (node->data == value) {
                    m_mutex.unlock();
                    return node;
                }
                node = node->next;
            }
            std::cerr << "No such element in the list \n";
            m_mutex.unlock();
            return nullptr;
        }

        void display(std::ostream& out = std::cout) const {
            m_mutex.lock();
            Node* node = m_head;
            while (node != nullptr) {
                out << node->data << " ";
                node = node->next;
            }
            m_mutex.unlock();
        }
};