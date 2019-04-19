#pragma once
#include <memory>
#include "comm.pb.h"

namespace chat {
    // Participant abstract class for defining send(...) interface
    class participant
    {
    public:
        virtual void send(const Message& msg) = 0;
        std::string get_nickname() const noexcept {return _nickname;}
        void set_nickname(const std::string& new_nick) noexcept {_nickname = new_nick;}
        virtual ~participant() {}

    protected:
        std::string _nickname;
    };

    using participant_ptr = std::shared_ptr<participant>;
}
