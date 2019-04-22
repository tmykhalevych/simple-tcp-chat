#pragma once
#include <sstream>
#include <string>

namespace chat {
    class exception
    {
    public:
        exception(const std::string& module, const std::string& message)
            :_module(module)
            ,_message(message)
        {}

        std::string what() const noexcept {
            std::ostringstream oss;
            oss << '[' << _module << "] " << _message ;
            return oss.str();
        }

    private:

        std::string _module;
        std::string _message;
    };
}
