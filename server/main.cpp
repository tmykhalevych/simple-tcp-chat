#include <boost/asio.hpp>
#include <iostream>
#include "server.h"
#include "exception.hpp"
#include "config.h"
#include "comm.pb.h"
#include "logger.h"

int main(int argc, char** argv)
{
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::string server_port = _CHAT_SERVER_DEFAULT_PORT_;
    if (argc >= 2) {
        server_port = argv[1];
    }

    try {
        LOG_GLOBAL_MSG("Start chatting server on " + server_port + " port...")

        boost::asio::io_service io;

        chat::server server(io, server_port);
        server.run();
    }
    catch (chat::exception& exp) {
        LOG_GLOBAL_EXP("Server exception happened: " + std::string(exp.what()))
        std::cerr << "Server exception happened: " << exp.what() << "\n";
    }

    // Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}