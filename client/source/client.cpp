#include "client.h"
#include "logger.h"
#include "message_helpers.inl"

namespace chat {
    client::client(std::string host, std::string port)
        : _io(1)
        , _socket(_io)
        , _resolver(_io)
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

    bool client::join_room(std::string nickname, std::string pass) {
        LOG_SCOPE
        // TODO
        return false;
    }

    void client::connect() {
        LOG_SCOPE
        boost::asio::async_connect(
            _socket,
            _endpoint,
            [this](boost::system::error_code ec, tcp::resolver::iterator) {
                if (!ec) {
                    read_header_and(std::bind(&client::read, this));
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
            }
        );
    }

    void client::read() {
        LOG_SCOPE
        boost::asio::async_read(
            _socket,
            boost::asio::buffer(_msg_buff, _msg_buff_size),
            [this](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    LOG_MSG("<--- Receive <message>")
                    LOG_MSG("message = " + std::string(_msg_buff, _msg_buff_size))
                    Message msg;
                    std::istringstream iss(std::string(_msg_buff, _msg_buff_size));
                    msg.ParseFromIstream(&iss);
                    _read_callback(msg);
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
                    LOG_MSG("---> Send message")
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
        if (_msg_buff != nullptr) free_msg_buff();
        _msg_buff_size = ntohl(*(std::int32_t*)_header_buff); // FIXME: Only linux!
        alloc_msg_buff(_msg_buff_size);
        LOG_MSG("header = " + std::to_string(_msg_buff_size))
    }
}
