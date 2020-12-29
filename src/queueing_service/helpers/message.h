#pragma once

#ifndef QUEUEING_SERVICE_HELPERS_MESSAGE_H
#define QUEUEING_SERVICE_HELPERS_MESSAGE_H

#include "enums.h"

namespace queueing_service {
	template <typename T> struct message {
		receiver m_receiver = receiver::client;
		T* m_data = nullptr;

		message() {}
		message(receiver t_receiver, T* t_data) {
			m_receiver = t_receiver;
			m_data = t_data;
		}

		bool operator==(const message& x) const {
			return (m_receiver == x.m_receiver && m_data == x.m_data);
		}
	};
}

#endif