#pragma once

#ifndef COMMON_HELPERS_BINARY_WRITER_H
#define COMMON_HELPERS_BINARY_WRITER_H

namespace common {
	class binary_writer {
		public:
			void write_int(char*& t_buffer, int t_value);
			void write_float(char*& t_buffer, float t_value);
			void write_double(char*& t_buffer, double t_value);
			void write_short(char*& t_buffer, short t_value);
			void write_char(char*& t_buffer, char* t_value, unsigned int t_length);
			void write_bool(char*& t_buffer, bool t_value);
	};

}

#endif