#pragma once

#ifndef COMMON_HELPERS_BINARY_READER_H
#define COMMON_HELPERS_BINARY_READER_H

namespace common {
	class binary_reader {
		public:
			int read_int(char*& t_buffer);
			float read_float(char*& t_buffer);
			double read_double(char*& t_buffer);
			short read_short(char*& t_buffer);
			bool read_bool(char*& t_buffer);
			char* read_char(char*& t_buffer, unsigned int t_length);
	};
}

#endif