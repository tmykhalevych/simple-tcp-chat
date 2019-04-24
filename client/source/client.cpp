#include "client.h"
#include "logger.h"
#include "message_helpers.inl"

// For ntohl(...) using
#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

namespace chat {
    client::client(boost::asio::io_service& io, std::string host, std::string port)
        : _io(io)
        , _socket(_io)
        , _resolver(_io)
        , _last_exp()
        , _msg_buff()
    {
        LOG_SCOPE
        _endpoint = _resolver.resolve({host, port});
        connect();
    }

    void client::send(Message& msg) {
        LOG_SCOPE
        bool not_writing = _message_q.empty();
        _message_q.push_back(msg);
        if (not_writing) {
            write();
        }
    }

    void client::join_room() {
        LOG_SCOPE

        Connect conn_req;
        auto user_data = _join_callback();
        conn_req.set_nickname(user_data.first);
        conn_req.set_password(user_data.second);

        boost::asio::streambuf output_buff;
        std::ostream os(&output_buff);
        message::add_header(&os, conn_req);
        conn_req.SerializeToOstream(&os);

        boost::asio::async_write(
            _socket,
            output_buff,
            [this](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    LOG_MSG("---> Send <connect>")
                    // TODO: Add connect containings log
                }
                else if (ec != boost::asio::error::operation_aborted) {
                    end();
                }
            }
        );
    }

    void client::wait_for_ack() {
        LOG_SCOPE
        boost::asio::async_read(
            _socket,
            boost::asio::buffer(_msg_buff.begin(), _msg_buff.size()),
            [this](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    LOG_MSG("<--- Receive <ack>")
                    std::string str_msg = std::string(_msg_buff.begin(), _msg_buff.size());
                    LOG_MSG("ack = " + str_msg)
                    Message msg;
                    std::istringstream iss(str_msg);
                    msg.ParseFromIstream(&iss);
                    if (msg.has_payload() && msg.payload() == "true") {
                        LOG_MSG("Gor acknoladge")
                        read_header_and(std::bind(&client::read, this));
                    }
                    else {
                        LOG_MSG("Gor reject. Try to connect once more")
                        _read_callback(msg);
                        _last_exp = client::exception("", 0);
                    }
                }
                else if (ec != boost::asio::error::operation_aborted) {
                    end();
                }
            }
        );
    }

    void client::connect() {
        LOG_SCOPE
        boost::asio::async_connect(
            _socket,
            _endpoint,
            [this](boost::system::error_code ec, tcp::resolver::iterator) {
                if (!ec) {
                    LOG_MSG("Connection with server has been established")
                    read_header_and(std::bind(&client::wait_for_ack, this));
                }
                else {
                    _last_exp = client::exception("ERROR - Server is down", 0);
                }
            }
        );
    }

    void client::read_header_and(std::function<void(void)> next_action) {
        LOG_SCOPE
        boost::asio::async_read(
            _socket,
            boost::asio::buffer(_header_buff, _header_buff_size),
            [this, next_action](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    LOG_MSG("<--- Receive <header>")
                    process_header();
                    next_action();
                }
                else if (ec != boost::asio::error::operation_aborted) {
                    end();
                }
            }
        );
    }

    void client::read() {
        LOG_SCOPE
        boost::asio::async_read(
            _socket,
            boost::asio::buffer(_msg_buff.begin(), _msg_buff.size()),
            [this](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    LOG_MSG("<--- Receive <message>")
                    std::string str_msg = std::string(_msg_buff.begin(), _msg_buff.size());
                    LOG_MSG("message = " + str_msg)
                    Message msg;
                    std::istringstream iss(str_msg);
                    msg.ParseFromIstream(&iss);
                    _read_callback(msg);
                    read_header_and(std::bind(&client::read, this));
                }
                else if (ec != boost::asio::error::operation_aborted) {
                    end();
                }
            }
        );
    }

    void client::write() {
        LOG_SCOPE
        boost::asio::streambuf output_buff;
        std::ostream os(&output_buff);
        message::add_header(&os, _message_q.front());
        _message_q.front().SerializeToOstream(&os);
        boost::asio::async_write(
            _socket,
            output_buff,
            [this](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    LOG_MSG("---> Send <message>")
                    // TODO: Add message containings log
                    _message_q.pop_front();
                    if (!_message_q.empty()) {
                        write();
                    }
                }
                else if (ec != boost::asio::error::operation_aborted) {
                    end();
                }
            }
        );
    }

    void client::process_header() {
        LOG_SCOPE
        _msg_buff.realloc(ntohl(*(std::int32_t*)_header_buff));
        LOG_MSG("header = " + std::to_string(_msg_buff.size()))
    }
}
