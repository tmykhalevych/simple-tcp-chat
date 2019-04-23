#pragma once
#include "participant.hpp"
#include "message_helpers.inl"
#include "comm.pb.h"
#include "config.h"
#include "logger.h"
#include <set>
#include <sstream>

namespace chat {
    // Room to keep all participants together
    class room
        : private Loggable
    {
        LOG_MODULE(" RM")
    public:
        room(const room&) = delete;
        room& operator=(const room&) = delete;

        room();

        void join(participant_ptr patricipant) noexcept;
        void kick(participant_ptr patricipant) noexcept;

        enum class validation {
            ok = 1,
            invalid_nickname,
            abscent_nickname,
            invalid_password,
            abscent_password,
            invalid_target
        };

        validation validate(Connect conn_req) noexcept;
        void notify(const Message& msg) noexcept;
        void send(const std::set<participant_ptr>& participants, const Message& msg) noexcept;
        validation route(Message& msg, const std::string& from) noexcept;

    private:
        std::set<participant_ptr> _participants;

    public:
        static Message get_err_msg(validation err) noexcept;
    };
}
