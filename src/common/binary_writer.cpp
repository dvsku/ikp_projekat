#include "pch.h"
#include "binary_writer.h"

#include <string.h>

namespace common {
	void binary_writer::write_int(char*& t_buffer, int t_value) {
		*(int*)t_buffer = t_value;
		t_buffer += sizeof(int);
	}

	void binary_writer::write_float(char*& t_buffer, float t_value) {
		*(float*)t_buffer = t_value;
		t_buffer += sizeof(float);
	}

	void binary_writer::write_double(char*& t_buffer, double t_value) {
		*(double*)t_buffer = t_value;
		t_buffer += sizeof(double);
	}

	void binary_writer::write_short(char*& t_buffer, short t_value) {
		*(short*)t_buffer = t_value;
		t_buffer += sizeof(short);
	}

	void binary_writer::write_char(char*& t_buffer, char* t_value, unsigned int t_length) {
		memcpy(t_buffer, t_value, t_length);
		t_buffer += t_length;
	}

	void binary_writer::write_bool(char*& t_buffer, bool t_value) {
		*(bool*)t_buffer = t_value;
		t_buffer += sizeof(bool);
	}
}