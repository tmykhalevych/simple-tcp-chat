#pragma once
#include "participant.hpp"
#include "room.hpp"
#include "comm.pb.h"
#include "config.h"
#include "logger.h"
#include <boost/asio.hpp>
#include <array>
#include <deque>
#include <functional>
#include <cstring>

using boost::asio::ip::tcp;

namespace chat {
    // Client connection handler
    class connection
        : public participant
        , public std::enable_shared_from_this<connection>
        , private Loggable
    {
        LOG_MODULE("CON")
    public:
        connection(const connection&) = delete;
        connection& operator=(const connection&) = delete;

        connection(tcp::socket sock, room& rm);
        ~connection();

        void establish();
        void send(const Message& msg) override;

    private:
        void read_header_and(std::function<void(void)> next_action);
        void set();
        void read();
        void write();

        void alloc_msg_buff(std::size_t buff_size) {
            LOG_SCOPE
            _msg_buff = (char*)malloc(buff_size);
        }
        void free_msg_buff() {
            LOG_SCOPE
            free(_msg_buff);
            _msg_buff = nullptr;
        }

        void process_header();

        tcp::socket _socket;
        room _room;
        static const int _header_buff_size = _CHAT_MSG_HEADER_SIZE_;
        char _header_buff[_header_buff_size];
        char* _msg_buff = nullptr;
        std::size_t _msg_buff_size;
        std::deque<Message> _message_q;
        std::string _client_addr;
    };
}
