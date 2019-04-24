#include <string>
#include <sstream>
#include <iostream>
#include <future>
#include <boost/asio.hpp>
#include "comm.pb.h"
#include "logger.h"
#include "comm.pb.h"
#include "client.h"
#include "config.h"
#include "message_helpers.inl"

int main(int argc, char** argv)
{
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::string server_host = _CHAT_SERVER_DEFAULT_HOST_;
    std::string server_port = _CHAT_SERVER_DEFAULT_PORT_;
    if (argc >= 3) {
        server_host = argv[1];
        server_port = argv[2];
    }

    try {
        // TODO: Add configuring host
        chat::client client(server_host, server_port);
        std::ostringstream log;
        log << "Start client. Try to connect to " << server_host << ":" << server_port << "...";
        LOG_GLOBAL_MSG(log.str())
        log.clear();

        // Set client read callback
        client.on_read_msg(
            [](chat::Message msg){
                std::ostringstream oss;
                oss << "[" 
                    << (msg.has_target() ? msg.target() : "ROOM")
                    << "] "
                    << msg.payload();
                std::cout << oss.str() << std::endl;
            }
        );

        // Set join callback
        std::string user_nickname, room_password;
        client.on_join_room(
            [&user_nickname, &room_password]() -> std::pair<std::string, std::string> {
                std::cout << "Please enter your nickname: ";
                std::cin >> user_nickname;
                std::cout << "Please enter the room password: ";
                std::cin >> room_password;
                // The lightest solution to avoid catching '\n' by next getline(...)
                std::cin.ignore(256, '\n');

                return std::make_pair(user_nickname, room_password);
            }
        );

        // Start client in other thread
        auto client_thr = std::async(
            std::launch::async,
            [&client] () -> void { client.run(); }
        );

        // Try to join chat room
        client.join_room();

        log << "Enter the chat room as [" << user_nickname << "]";
        LOG_GLOBAL_MSG(log.str())
        log.clear();

        std::string msg_str;
        while (true)
        {
            // Get exception from the client_thr
            if (auto ep = client.get_last_exp()) {
                throw ep;
            }

            std::getline(std::cin, msg_str);
            if (msg_str == "/q") {
                break;
            }
            else {
                chat::Message msg = chat::message::parce_from_string(msg_str);
                client.send(msg);
            }
        }

        client.end();
        client_thr.get();
    }
    catch (chat::client::exception& e) {
        LOG_GLOBAL_EXP(e.what())
        std::cout << e.what() << std::endl;
        return e.code();
    }
    catch (std::exception& e) {
        LOG_GLOBAL_EXP(std::string(e.what()))
        std::cerr << "Exception: " << e.what() << "\n";
    }

    // Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}