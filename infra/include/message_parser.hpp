#pragma once

namespace chat::message {
    struct parser
    {
        parser() noexcept {
            reset();
        }

        void parse(const std::string& raw, std::string& target, std::string& message) {
            for (char elem: raw) {
                consume(elem);
            }

            target = _target;
            message = _message;
            reset();
        }
    
    private:
        void consume(char input) noexcept {
            switch (_state) {
            case state::start:
                if (input == ' ') {
                    _state = state::start;
                }
                else if (input == '@') {
                    _state = state::target;
                }
                else {
                    _message.push_back(input);
                    _state = state::message;
                }
                break;
            case state::target:
                if (input == ' ') {
                    _state = state::expecting_message_start;
                }
                else {
                    _target.push_back(input);
                }
                break;
            case state::expecting_message_start:
                if (input == ' ') {
                    _state = state::expecting_message_start;
                }
                else {
                    _message.push_back(input);
                    _state = _state = state::message;
                }
                break;
            case state::message:
                _message.push_back(input);
                break;
            }
        }

        void reset() noexcept {
            _state = state::start;
            _target = "";
            _message = "";
        }

        enum class state {
            start,
            target,
            expecting_message_start,
            message
        } _state;
        std::string _target;
        std::string _message;
    };
}
