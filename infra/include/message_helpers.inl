#pragma once
#include <string>
#include <sstream>
#include <ostream>
#include "comm.pb.h"
#include <arpa/inet.h> // FIXME: Only linux
#include "message_parser.hpp"
#include "logger.h"


namespace chat::message {
    inline Message from_string(const std::string& str) {
        Message msg;
        msg.set_payload(str);
        return msg;
    }

    template<typename MESSAGE>
    inline void add_header(std::ostream* os, MESSAGE msg) {
        std::int32_t msg_size = static_cast<std::int32_t>(htonl(msg.ByteSizeLong())); // FIXME: Only linux
        char* byte = (char*)&msg_size;
        (*os) << byte[0] << byte[1] << byte[2] << byte[3];
    }

    inline Message parce_from_string(const std::string& str) {
        Message msg;
        std::string target = "";
        std::string message = "";
        parser p;
        p.parse(str, target, message);
        std::ostringstream log;
        log << "raw string message: " << str << "; target: " << target << "; message: " << message;
        LOG_GLOBAL_MSG(log.str())
        if (target != "") msg.set_target(target);
        msg.set_payload(message);
        return msg;
    }
}
