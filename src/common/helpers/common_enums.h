#pragma once

#ifndef COMMON_HELPERS_ENUMS_H
#define COMMON_HELPERS_ENUMS_H

namespace common {

	// MESSAGE STRUCTURE
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	HEADER (6 bytes total)
	//	message_type(2 bytes) command(2 bytes) data_length(2 bytes)
	//	
	//	DATA
	//	data(data_length bytes)
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class command : short {
		ctos_connect_to_queue,					// request to connect to a specific queue
		ctos_queue_client_exists,				// request to check if a queue by a give name has a client connected on the other service
		ctos_send_message,						// request to send a message to the connected queue

		stoc_connect_to_queue,					// response if connection was successful or not
		stoc_queue_client_exists,				// response if the queue has a client connected on the other service
		stoc_send_message,						// message was sent by the other client

		stos_client_connected_to_queue,			// sent when a client has connected to a queue
		stos_client_disconnected_from_queue,	// sent when a client has disconnected from a queue
		stos_send_message						// send a message to the other service
	};

	enum class message_type : short {
		t_int,
		t_float,
		t_double,
		t_short,
		t_char,
		t_bool
	};

	enum class queue_type : short {
		t_int,
		t_float,
		t_double,
		t_short,
		t_char,
	};
};

#endif