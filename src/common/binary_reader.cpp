#include "pch.h"
#include "binary_reader.h"

#include <string.h>

namespace common {
	int binary_reader::read_int(char*& t_buffer) {
		int result = *(int*)t_buffer;
		t_buffer += sizeof(int);
		return result;
	}

	float binary_reader::read_float(char*& t_buffer) {
		float result = *(float*)t_buffer;
		t_buffer += sizeof(float);
		return result;
	}

	double binary_reader::read_double(char*& t_buffer) {
		double result = *(double*)t_buffer;
		t_buffer += sizeof(double);
		return result;
	}

	short binary_reader::read_short(char*& t_buffer) {
		short result = *(short*)t_buffer;
		t_buffer += sizeof(short);
		return result;
	}

	char* binary_reader::read_char(char*& t_buffer, unsigned int t_length) {
		char* result = new char[t_length];
		strcpy_s(result, t_length, t_buffer);
		t_buffer += t_length;
		return result;
	}
}