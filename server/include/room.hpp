#pragma once
#include "participant.hpp"
#include "message.h"
#include <set>

namespace chat {
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

        void notify(const message& message) const noexcept {
            for(auto patricipant: _participants) {
                patricipant->send(message);
            }
        }

    private:
        std::set<participant_ptr> _participants;
    };
}
