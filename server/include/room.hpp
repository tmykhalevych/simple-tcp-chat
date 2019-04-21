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
        void join(participant_ptr patricipant) noexcept {
            LOG_SCOPE
            _participants.insert(patricipant);
            std::ostringstream oss;
            oss << "New participant joined the room. Welcome @" << patricipant->get_nickname();
            notify(message::from_string(oss.str()));
            LOG_MSG("<< @" + patricipant->get_nickname() + " entered")
        }

        void kick(participant_ptr patricipant) noexcept {
            LOG_SCOPE
            _participants.erase(patricipant);
            LOG_MSG(">> @" + patricipant->get_nickname() + " kiked out")
        }

        enum class validation;
        validation validate(Connect conn_req) noexcept {
            LOG_SCOPE
            // TODO:
            // 1) Chack if password is correct
            // 2) Check if patricipant name is uniq
            return validation::ok;
        }

        void notify(const Message& msg) noexcept {
            LOG_SCOPE
            for(auto patricipant: _participants) {
                patricipant->send(msg);
            }
        }

        validation route(Message& msg, const std::string& from) noexcept {
            LOG_SCOPE
            std::string target_nickname = msg.target();
            auto target = std::find_if(
                _participants.begin(), 
                _participants.end(),
                [target_nickname](auto elem){ return elem->get_nickname() == target_nickname; }
            );
            if (target != _participants.end()) {
                msg.set_target(from);
                (*target)->send(msg);
                return validation::ok;
            }
            else {
                return validation::invalid_target;
            }
        }

    private:
        std::set<participant_ptr> _participants;

    public:
        enum class validation {
            ok = 1,
            invalid_nickname,
            invalid_password,
            invalid_target
        };

        static Message get_err_msg(validation err) noexcept {
            Message err_msg;

            switch (err) {
            case validation::invalid_nickname :
                err_msg = message::from_string("The nickname should be unique!");
                break;
            case validation::invalid_password :
                err_msg = message::from_string("Invalid password!");
                break;
            case validation::invalid_target :
                err_msg = message::from_string("There is no such participant in the room.");
                break;
            default :
                err_msg = message::from_string("Some error occured...");
            }

            return err_msg;
        }
    };
}
