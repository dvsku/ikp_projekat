#pragma once

#ifndef QUEUEING_SERVICE_HELPERS_MESSAGE_H
#define QUEUEING_SERVICE_HELPERS_MESSAGE_H

#include "enums.h"

namespace queueing_service {
	template <typename T> struct message {
		receiver m_receive = receiver::client;
		T* m_data = nullptr;

		message() {}
		message(receiver receiver_, T* data_) {
			m_receive = receiver_;
			m_data = data_;
		}

		bool operator==(const message& x) const {
			return (m_receive == x.m_receive && m_data == x.m_data);
		}
	};
}

#endif