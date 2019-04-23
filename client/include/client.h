#pragma once
#include "logger.h"
#include <boost/asio.hpp>
#include <functional>
#include "config.h"
#include "comm.pb.h"

using boost::asio::ip::tcp;

namespace chat {
    class client
        : private Loggable
    {
        LOG_MODULE("CLN")
    public:
        client(std::string host, std::string port);

        void send(Message& msg);
        void join_room();

        void on_read_msg(std::function<void(chat::Message)> read_callback) noexcept {
            _read_callback = read_callback;
        }

        void on_join_room(std::function<std::pair<std::string, std::string>()> join_callback) noexcept {
            _join_callback = join_callback;
        }

        void run() {
            _io.run();
        }

        void end() {
            _io.post(
                [this](){ _socket.close(); }
            );
            _last_exp = client::exception("Connection is broken", 0);
        }

        struct exception {
            exception(): _real(false) {}
            exception(std::string msg, int err_code): _msg(msg), _code(err_code), _real(true) {}
            std::string what() const noexcept { return _msg; }
            int code() const noexcept { return _code; }
            operator bool() { return _real; }
        private:
            std::string _msg;
            int _code;
            bool _real;
        };

        exception get_last_exp() const noexcept {
            return _last_exp;
        }

    private:
        void connect();
        void wait_for_ack();
        void read_header_and(std::function<void(void)> next_action);
        void read();
        void write();

        void alloc_msg_buff(std::size_t buff_size) {
            LOG_SCOPE
            _msg_buff = (char*)malloc(buff_size);
            // TODO: handle if malloc return NULL
            _msg_buff_size = buff_size;
        }
        void free_msg_buff() {
            LOG_SCOPE
            free(_msg_buff);
            _msg_buff = nullptr;
            _msg_buff_size = 0;
        }

        void process_header();

        boost::asio::io_service _io;
        tcp::resolver _resolver;
        tcp::resolver::iterator _endpoint;
        tcp::socket _socket;
        std::function<void(chat::Message)> _read_callback;
        std::function<std::pair<std::string, std::string>()> _join_callback;
        static const int _header_buff_size = _CHAT_MSG_HEADER_SIZE_;
        char _header_buff[_header_buff_size];
        char* _msg_buff = nullptr;
        std::size_t _msg_buff_size;
        std::deque<Message> _message_q;
        exception _last_exp;
    };
}
