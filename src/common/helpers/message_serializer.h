#pragma once

#ifndef COMMON_HELPERS_MESSAGE_SERIALIZER_H
#define COMMON_HELPERS_MESSAGE_SERIALIZER_H

#include <WinSock2.h>
#include <string.h>

#include "common_enums.h"
#include "binary_writer.h"

namespace {
    static char* serialize_message(common::message_type t_type, common::command t_command, void* t_data, unsigned int* t_total_size) {
        unsigned int size_header = 3 * sizeof(short);
        unsigned int size_data, total_size;

        common::binary_writer binary_writer{};

        switch (t_type) {
            case common::message_type::t_int: {
                int* msg_data = (int*)t_data;
                size_data = sizeof(int);
                total_size = size_header + size_data;

                char* message = new char[total_size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)t_type));
                binary_writer.write_short(pos, htons((short)t_command));
                binary_writer.write_short(pos, htons((short)sizeof(float)));
                binary_writer.write_int(pos, htonl(*msg_data));

                *t_total_size = total_size;
                return message;
            }
            case common::message_type::t_float: {
                float* msg_data = (float*)t_data;
                size_data = sizeof(float);
                total_size = size_header + size_data;

                char* message = new char[total_size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)t_type));
                binary_writer.write_short(pos, htons((short)t_command));
                binary_writer.write_short(pos, htons((short)sizeof(int)));
                binary_writer.write_float(pos, htonf(*msg_data));

                *t_total_size = total_size;
                return message;
            }
            case common::message_type::t_double: {
                double* msg_data = (double*)t_data;
                size_data = sizeof(double);
                total_size = size_header + size_data;

                char* message = new char[total_size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)t_type));
                binary_writer.write_short(pos, htons((short)t_command));
                binary_writer.write_short(pos, htons((short)sizeof(double)));
                binary_writer.write_double(pos, htond(*msg_data));

                *t_total_size = total_size;
                return message;
            }
            case common::message_type::t_short: {
                short* msg_data = (short*)t_data;
                size_data = sizeof(short);
                total_size = size_header + size_data;

                char* message = new char[total_size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)t_type));
                binary_writer.write_short(pos, htons((short)t_command));
                binary_writer.write_short(pos, htons((short)sizeof(short)));
                binary_writer.write_short(pos, htons(*msg_data));

                *t_total_size = total_size;
                return message;
            }
            case common::message_type::t_char: {
                char* msg_data = (char*)t_data;
                size_data = strlen(msg_data) + 1;
                total_size = size_header + size_data;

                char* message = new char[total_size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)t_type));
                binary_writer.write_short(pos, htons((short)t_command));
                binary_writer.write_short(pos, htons((short)size_data));
                binary_writer.write_char(pos, msg_data, size_data);

                *t_total_size = total_size;
                return message;
            }
            case common::message_type::t_bool: {
                bool* msg_data = (bool*)t_data;
                size_data = sizeof(bool);
                total_size = size_header + size_data;

                char* message = new char[total_size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)t_type));
                binary_writer.write_short(pos, htons((short)t_command));
                binary_writer.write_short(pos, htons((short)size_data));
                binary_writer.write_bool(pos, *msg_data);

                *t_total_size = total_size;
                return message;
            }
            default: return nullptr;
        }
    }
}

#endif