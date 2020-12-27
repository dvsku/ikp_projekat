#pragma once

#ifndef QUEUEING_SERVICE_HELPERS_MESSAGE_SERIALIZER_H
#define QUEUEING_SERVICE_HELPERS_MESSAGE_SERIALIZER_H

#include <WinSock2.h>
#include <string.h>

#include "common_enums.h"
#include "binary_writer.h"

namespace queueing_service {
    static char* serialize_message_to_service(common::message_type type, void* data, unsigned int* total_size) {
        unsigned int size_type = sizeof(short), size_len = sizeof(short);
        unsigned int size_data, size;

        common::binary_writer binary_writer{};

        switch (type) {
            case common::message_type::t_int: {
                int* msg_data = (int*)data;
                size_data = sizeof(int);
                size = size_type + size_len + size_data;

                char* message = new char[size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)common::message_type::t_int));
                binary_writer.write_short(pos, htons((short)sizeof(float)));
                binary_writer.write_int(pos, htonl(*msg_data));

                *total_size = size;
                return message;
            }
            case common::message_type::t_float: {
                float* msg_data = (float*)data;
                size_data = sizeof(float);
                size = size_type + size_len + size_data;

                char* message = new char[size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)common::message_type::t_float));
                binary_writer.write_short(pos, htons((short)sizeof(int)));
                binary_writer.write_float(pos, htonf(*msg_data));

                *total_size = size;
                return message;
            }
            case common::message_type::t_double: {
                double* msg_data = (double*)data;
                size_data = sizeof(double);
                size = size_type + size_len + size_data;

                char* message = new char[size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)common::message_type::t_double));
                binary_writer.write_short(pos, htons((short)sizeof(double)));
                binary_writer.write_double(pos, htond(*msg_data));

                *total_size = size;
                return message;
            }
            case common::message_type::t_short: {
                short* msg_data = (short*)data;
                size_data = sizeof(short);
                size = size_type + size_len + size_data;

                char* message = new char[size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)common::message_type::t_short));
                binary_writer.write_short(pos, htons((short)sizeof(short)));
                binary_writer.write_short(pos, htons(*msg_data));

                *total_size = size;
                return message;
            }
            case common::message_type::t_char: {
                char* msg_data = (char*)data;
                size_data = strlen(msg_data) + 1;
                size = size_type + size_len + size_data;

                char* message = new char[size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)common::message_type::t_char));
                binary_writer.write_short(pos, htons((short)size_data));
                binary_writer.write_char(pos, msg_data, size_data);

                *total_size = size;
                return message;
            }
            default: return nullptr;
        }
    }

    static char* serialize_response_to_client(common::response_from_service t_response_type, bool t_response, unsigned int* t_total_size) {
        unsigned int size_type = sizeof(short), size_cmd = sizeof(short), size_len = sizeof(short), size_response = sizeof(bool);
        unsigned int total_size = size_type + size_cmd + size_len + size_response;

        common::binary_writer binary_writer{};

        char* response = new char[total_size];
        char* pos = response;

        binary_writer.write_short(pos, htons((short)common::message_type::t_bool));
        binary_writer.write_short(pos, htons((short)t_response_type));
        binary_writer.write_short(pos, htons((short)size_len));
        binary_writer.write_bool(pos, t_response);

        *t_total_size = total_size;
        return response;
    }

    static char* serialize_message_to_client(common::message_type t_type, void* t_data, unsigned int* t_total_size) {
        unsigned int size_type = sizeof(short), size_cmd = sizeof(short), size_len = sizeof(short);
        unsigned int size_data, total_size;

        common::binary_writer binary_writer{};

        switch (t_type) {
            case common::message_type::t_int: {
                int* msg_data = (int*)t_data;
                size_data = sizeof(int);
                total_size = size_type + size_cmd + size_len + size_data;

                char* message = new char[total_size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)common::message_type::t_int));
                binary_writer.write_short(pos, htons((short)common::response_from_service::send_message));
                binary_writer.write_short(pos, htons((short)sizeof(int)));
                binary_writer.write_int(pos, htonl(*msg_data));

                *t_total_size = total_size;
                return message;
            }
            case common::message_type::t_float: {
                float* msg_data = (float*)t_data;
                size_data = sizeof(float);
                total_size = size_type + size_cmd + size_len + size_data;

                char* message = new char[total_size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)common::message_type::t_float));
                binary_writer.write_short(pos, htons((short)common::response_from_service::send_message));
                binary_writer.write_short(pos, htons((short)sizeof(float)));
                binary_writer.write_float(pos, htonf(*msg_data));

                *t_total_size = total_size;
                return message;
            }
            case common::message_type::t_double: {
                double* msg_data = (double*)t_data;
                size_data = sizeof(double);
                total_size = size_type + size_cmd + size_len + size_data;

                char* message = new char[total_size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)common::message_type::t_double));
                binary_writer.write_short(pos, htons((short)common::response_from_service::send_message));
                binary_writer.write_short(pos, htons((short)sizeof(double)));
                binary_writer.write_double(pos, htond(*msg_data));

                *t_total_size = total_size;
                return message;
            }
            case common::message_type::t_short: {
                short* msg_data = (short*)t_data;
                size_data = sizeof(short);
                total_size = size_type + size_cmd + size_len + size_data;

                char* message = new char[total_size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)common::message_type::t_short));
                binary_writer.write_short(pos, htons((short)common::response_from_service::send_message));
                binary_writer.write_short(pos, htons((short)sizeof(short)));
                binary_writer.write_short(pos, htons(*msg_data));

                *t_total_size = total_size;
                return message;
            }
            case common::message_type::t_char: {
                char* msg_data = (char*)t_data;
                size_data = strlen(msg_data) + 1;
                total_size = size_type + size_cmd + size_len + size_data;

                char* message = new char[total_size];
                char* pos = message;

                binary_writer.write_short(pos, htons((short)common::message_type::t_char));
                binary_writer.write_short(pos, htons((short)common::response_from_service::send_message));
                binary_writer.write_short(pos, htons((short)size_data));
                binary_writer.write_char(pos, msg_data, size_data);

                *t_total_size = total_size;
                return message;
            }
            default: return nullptr;
        }
    }
}

#endif