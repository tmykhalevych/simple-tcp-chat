#pragma once
#include <string>
#include "comm.pb.h"

namespace chat {
    namespace message {
        inline Message from_string(const std::string& str) {
            Message msg;
            msg.set_payload(str);
            return msg;
        }
    };
}
