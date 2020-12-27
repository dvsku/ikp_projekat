#pragma once

#ifndef COMMON_HELPERS_ENUMS_H
#define COMMON_HELPERS_ENUMS_H

namespace common {
	enum class request_to_service : short {
		connect_to_queue,		// request to connect to a specific queue
		queue_exists,			// request to check if a queue by a given name exists
		send_message			// request to send a message to the connected queue
	};
	enum class response_from_service : short {
		connect_to_queue,		// response if connection was successful or not
		queue_exists,			// response if the queue exists or not
		send_message			// message has been sent by the other client
	};
	enum class message_type : short {
		t_int,
		t_float,
		t_double,
		t_short,
		t_char,
		t_bool
	};
};

#endif