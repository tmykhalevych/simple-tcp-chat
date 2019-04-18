#pragma once
#include <boost/asio.hpp>
#include "room.hpp"

using boost::asio::ip::tcp;

namespace chat {
    class server
    {
    public:
        server(const server&) = delete;
        server& operator=(const server&) = delete;

        server(boost::asio::io_service& io, const unsigned int port);
        void run();
    
    private:
        void accept();

        const int _port;
        room _room;
        tcp::endpoint _endpoint;
        tcp::acceptor _acceptor;
        tcp::socket _socket;
    };
}
