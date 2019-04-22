#pragma once
#include <string>
#include <ostream>
#include "comm.pb.h"

namespace chat::message {
    inline Message from_string(const std::string& str) {
        Message msg;
        msg.set_payload(str);
        return msg;
    }

    inline void add_header(std::ostream* os, Message msg) {
        std::int32_t msg_size = static_cast<std::int32_t>(msg.ByteSizeLong());
        char* byte = (char*)&msg_size;
        (*os) << byte[0] << byte[1] << byte[2] << byte[3];
    }
}
