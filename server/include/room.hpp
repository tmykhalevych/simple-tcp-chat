#pragma once
#include "participant.hpp"
#include "message.h"
#include <set>

namespace chat {
    enum class validation {
        ok = 1,
        invalid_nickname
    };

    // Room to keep all participants together
    class room
    {
    public:
        void join(participant_ptr patricipant) noexcept {
            _participants.insert(patricipant);
        }

        void kick(participant_ptr patricipant) noexcept {
            _participants.erase(patricipant);
        }

        validation validate(participant_ptr participant) noexcept {
            // TODO: Check if patricipant name is uniq
            return validation::ok;
        }

        void notify(const message& message) const noexcept {
            for(auto patricipant: _participants) {
                patricipant->send(message);
            }
        }

        message get_err_msg(validation err) const noexcept {
            message err_msg;
            switch (err) {
            case validation::invalid_nickname :
                err_msg = message::invalid_nickname();
                break;
            default :
                err_msg = message::invalid_err();
            }
            return err_msg;
        }

    private:
        std::set<participant_ptr> _participants;
    };
}
