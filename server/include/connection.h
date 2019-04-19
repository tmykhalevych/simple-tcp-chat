#pragma once
#include "participant.hpp"
#include "room.hpp"
#include <boost/asio.hpp>
#include <deque>

using boost::asio::ip::tcp;

namespace chat {
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
        void send(const message& msg) override;

    private:
        void read();
        void write();

        tcp::socket _socket;
        room _room;
        std::deque<message> _message_q;
    };
}
