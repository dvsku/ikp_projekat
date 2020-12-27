#pragma once

#ifndef COMMON_HELPERS_LINKED_LIST_H
#define COMMON_HELPERS_LINKED_LIST_H

#include "pch.h"

#include <mutex>

namespace common {
    template <typename T> class linked_list {
        private:
            template <typename T> struct Node {
                T data;
                Node* next = nullptr;
            };

            unsigned int m_length;
            Node<T>* m_head;
            Node<T>* m_tail;
            std::mutex m_mutex;

        public:
            linked_list();
            ~linked_list();
            bool is_empty();
            unsigned int length();
            void prepend_node(T value);
            void append_node(T value);
            T take_first_item();
            void remove_item_at_index(unsigned int index);
            void remove_item_by_value(T value);
            T* get_item_at_index(unsigned int index);

        private:
            bool p_is_empty();
            unsigned int p_length();
            void p_prepend_node(T data);
            void p_append_node(T value);
            T p_take_first_item();
            void p_remove_item_at_index(unsigned int index);
            void p_remove_item_by_value(T value);
            T* p_get_item_at_index(unsigned int index);
    };
}

#include "linked_list.cpp"

#endif