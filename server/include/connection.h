#pragma once
#include "participant.hpp"
#include "room.hpp"
#include "comm.pb.h"
#include <boost/asio.hpp>
#include <array>
#include <deque>

using boost::asio::ip::tcp;

namespace chat {
    const unsigned int input_buff_dim   = 8192; // 8 KB
	using input_buff_type               = std::array<char, input_buff_dim>;
    using input_buff_size               = std::size_t;
    using message_q_type                = std::deque<Message>;

    // Client connection handler
    class connection
        : public participant // to override send(...)
        , public std::enable_shared_from_this<connection> // to pass into callbacks
    {
    public:
        connection(const connection&) = delete;
        connection& operator=(const connection&) = delete;

        connection(tcp::socket sock, room& rm);

        void establish();
        void send(const Message& msg) override;

    private:
        void read();
        void write();

        tcp::socket _socket;
        room _room;
        input_buff_type _input_buff;
        input_buff_size _input_buff_size;
        message_q_type _message_q;
    };
}
