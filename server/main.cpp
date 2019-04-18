#include <boost/asio.hpp>
#include <iostream> // Only for cerr
#include "server.h"
#include "exception.hpp"
#include "config.h" // FIXME

int main()
{
    // TODO: Add reading configuration file instead of hardcoded values

    // IO object should be global per thread
    boost::asio::io_service io_service;

    try {
        chat::server server(io_service, _CHAT_SERVER_PORT_);
        server.run();
    }
    catch (chat::exception& exp) {
        std::cerr << "Server exception happened: " << exp.what() << "\n";
    }

    return 0;
}