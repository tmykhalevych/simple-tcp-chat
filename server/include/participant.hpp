#pragma once
#include <memory>
#include "message.h"

namespace chat {
    // Participant abstract class for defining send(...) interface
    class participant
    {
    public:
        virtual void send(const message& msg) = 0;
        virtual ~participant() {}
    };

    using participant_ptr = std::shared_ptr<participant>;
}
