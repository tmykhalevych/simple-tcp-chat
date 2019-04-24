#include "room.h"

namespace chat {
    room::room() {
        LOG_SCOPE
        LOG_MSG("Chat room has been created")
    }

    void room::join(participant_ptr patricipant) noexcept {
        LOG_SCOPE
        _participants.insert(patricipant);
        std::ostringstream oss;
        oss << "New participant joined the room. Welcome @" << patricipant->get_nickname();
        notify(message::from_string(oss.str()));
        LOG_MSG("<< @" + patricipant->get_nickname() + " entered")
    }

    void room::kick(participant_ptr patricipant) noexcept {
        LOG_SCOPE
        std::ostringstream oss;
        oss << "Participant @" << patricipant->get_nickname() << " has been kicked from the room. Good bye.";
        notify(message::from_string(oss.str()));
        patricipant->release();
        _participants.erase(patricipant);
        LOG_MSG(">> @" + patricipant->get_nickname() + " kiked out")
    }

    room::validation room::validate(Connect conn_req) noexcept {
        LOG_SCOPE
        // Nickname validation
        if (!conn_req.has_nickname()) {
            return room::validation::abscent_nickname;
        }
        else {
            std::string nickname = conn_req.nickname();
            auto participant = find_if(
                _participants.begin(),
                _participants.end(),
                [nickname](auto elem){ return elem->get_nickname() == nickname; }
            );
            if (participant != _participants.end()) {
                // Nickname is not unique
                return room::validation::invalid_nickname;
            }
        }

        // Password validation
        if (!conn_req.has_password()) {
            return room::validation::abscent_password;
        }
        else if (conn_req.password() != _CHAT_SERVER_ENTER_PASSWORD_) {
            return room::validation::invalid_password;
        }

        return room::validation::ok;
    }

    void room::notify(const Message& msg) noexcept {
        LOG_SCOPE
        for(auto patricipant: _participants) {
            patricipant->send(msg);
        }
    }

    void room::send(const std::set<participant_ptr>& participants, const Message& msg) noexcept {
        LOG_SCOPE
        for(auto patricipant: participants) {
            patricipant->send(msg);
        }
    }

    room::validation room::route(Message& msg, const std::string& from) noexcept {
        LOG_SCOPE
        if (msg.has_target()) {
            if ((msg.target() == "kick") && (from == "Admin")) { // Admin wants to kick somebody
                std::string target_nickname = msg.payload();
                // Search user to kick it from the chat
                auto target = std::find_if(
                    _participants.begin(), 
                    _participants.end(),
                    [target_nickname](auto elem){ return elem->get_nickname() == target_nickname; }
                );
                if (target != _participants.end()) {
                    // Kick user from the chat
                    kick(*target);
                    return room::validation::ok;
                }
                else {
                    // Find admin and say that there is no such user
                    auto target = std::find_if(
                        _participants.begin(), 
                        _participants.end(),
                        [target_nickname](auto elem){ return elem->get_nickname() == "Admin"; }
                    );
                    if (target != _participants.end()) { // Just in case
                        std::ostringstream oss;
                        oss << "There is no user with nick: " << target_nickname;
                        msg.clear_target();
                        msg.set_payload(oss.str());
                        (*target)->send(msg);
                        return room::validation::ok;
                    }
                }
            }
            else {
                // addon: Sending message to separate client
                std::string target_nickname = msg.target();
                auto target = std::find_if(
                    _participants.begin(), 
                    _participants.end(),
                    [target_nickname](auto elem){ return elem->get_nickname() == target_nickname; }
                );
                if (target != _participants.end()) {
                    std::ostringstream oss;
                    oss << from << ":private";
                    msg.set_target(oss.str());
                    (*target)->send(msg);
                    return room::validation::ok;
                }
                else {
                    return room::validation::invalid_target;
                }
            }
        }
        else {
            msg.set_target(from);
            auto others = _participants; // To erase itself
            auto self = std::find_if(
                others.begin(), 
                others.end(),
                [from](auto elem){ return elem->get_nickname() == from; }
            );
            if (self != _participants.end()) { // Just in case
                others.erase(self);
            }
            send(others, msg);
            return room::validation::ok;
        }
    }

    Message room::get_err_msg(room::validation err) noexcept {
        Message err_msg;

        switch (err) {
        case room::validation::invalid_nickname :
            err_msg = message::from_string("The nickname should be unique!");
            break;
        case room::validation::abscent_nickname :
            err_msg = message::from_string("You should specify the nickname!");
            break;
        case room::validation::abscent_password :
        case room::validation::invalid_password :
            err_msg = message::from_string("Invalid password!");
            break;
        case room::validation::invalid_target :
            err_msg = message::from_string("There is no such participant in the room.");
            break;
        default :
            err_msg = message::from_string("Some error occured...");
        }

        return err_msg;
    }

}
