#include <boost/asio.hpp>
#include <iostream> // Only for cerr
#include "server.h"
#include "exception.hpp"
#include "config.h" // FIXME
#include "comm.pb.h"

int main()
{
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

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

    // Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}