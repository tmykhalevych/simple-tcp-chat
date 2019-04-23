#include <boost/asio.hpp>
#include <iostream>
#include "server.h"
#include "exception.hpp"
#include "config.h"
#include "comm.pb.h"
#include "logger.h"

int main()
{
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // TODO: Add reading configuration file instead of hardcoded values

    try {
        LOG_GLOBAL_MSG("Start chatting server on " + std::to_string(_CHAT_SERVER_PORT_) + " port...")
        chat::server server(_CHAT_SERVER_PORT_);
        server.run();
    }
    catch (chat::exception& exp) {
        LOG_GLOBAL_EXP(std::string("Server exception happened: ") + exp.what())
        std::cerr << "Server exception happened: " << exp.what() << "\n";
    }

    // Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}