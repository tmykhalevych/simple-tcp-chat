#pragma once
#include <string>

namespace chat {
    // Wrapper for raw string messages
    class message
    {
    public:
        message(): _raw("") {}
        explicit message(const std::string& raw): _raw(raw) {}

        std::string destination() const noexcept;
        std::string serialize() const;
        bool empty() const noexcept {return _raw.empty();}
        std::string to_str() const noexcept {return _raw;}

    private:
        std::string _raw;
    };
}
