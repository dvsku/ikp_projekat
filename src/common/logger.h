#pragma once
#pragma warning(disable:4996)

#ifndef COMMON_HELPERS_LOGGER_H
#define COMMON_HELPERS_LOGGER_H

#define LOG_INFO(source, text, ...) logger_info(source, text, ##__VA_ARGS__)
#define LOG_ERROR(source, text, ...) logger_error(source, text, ##__VA_ARGS__)
#define LOG_WARNING(source, text, ...) logger_warning(source, text, ##__VA_ARGS__)


#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <iostream>
#include <string>

namespace {
	template<typename... Args> static void logger_info(const char* source, const char* text, Args... args) {
		char		buffer[1024]{};
		time_t		now = time(0);
		struct tm	tstruct;
		char		time_buffer[80];
		localtime_s(&tstruct, &now);
		strftime(time_buffer, sizeof(time_buffer), "[%X][", &tstruct);
		
		int len = 0;

		char* pos = buffer;
		len = strlen(time_buffer);
		strncpy(pos, time_buffer, len);
		pos += len;
		strncpy(pos, "INFO][", 6);
		pos += 6;
		len = strlen(source);
		strncpy(pos, source, len);
		pos += len;
		strncpy(pos, "] ", 2);
		pos += 2;
		len = strlen(text);
		strncpy(pos, text, len);
		pos += len;
		strncpy(pos, "\n", 2);

		printf(buffer, args...);
	}

	template<typename... Args> static void logger_error(const char* source, const char* text, Args... args) {
		char		buffer[1024]{};
		time_t		now = time(0);
		struct tm	tstruct;
		char		time_buffer[80];
		localtime_s(&tstruct, &now);
		strftime(time_buffer, sizeof(time_buffer), "[%X][", &tstruct);

		int len = 0;

		char* pos = buffer;
		len = strlen(time_buffer);
		strncpy(pos, time_buffer, len);
		pos += len;
		strncpy(pos, "ERROR][", 7);
		pos += 7;
		len = strlen(source);
		strncpy(pos, source, len);
		pos += len;
		strncpy(pos, "] ", 2);
		pos += 2;
		len = strlen(text);
		strncpy(pos, text, len);
		pos += len;
		strncpy(pos, "\n", 2);

		printf(buffer, args...);
	}

	template<typename... Args> static void logger_warning(const char* source, const char* text, Args... args) {
		char		buffer[1024]{};
		time_t		now = time(0);
		struct tm	tstruct;
		char		time_buffer[80];
		localtime_s(&tstruct, &now);
		strftime(time_buffer, sizeof(time_buffer), "[%X][", &tstruct);

		int len = 0;

		char* pos = buffer;
		len = strlen(time_buffer);
		strncpy(pos, time_buffer, len);
		pos += len;
		strncpy(pos, "WARNING][", 9);
		pos += 9;
		len = strlen(source);
		strncpy(pos, source, len);
		pos += len;
		strncpy(pos, "] ", 2);
		pos += 2;
		len = strlen(text);
		strncpy(pos, text, len);
		pos += len;
		strncpy(pos, "\n", 2);

		printf(buffer, args...);
	}
}

#endif