#include "client.h"
#include "logger.h"

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
        // TODO
    }

    bool client::join_room(std::string nickname, std::string pass) {
        LOG_SCOPE
        // TODO
        return false;
    }

    void client::connect() {
        LOG_SCOPE
        // TODO
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
        // TODO
    }

    void client::write() {
        LOG_SCOPE
        // TODO
    }

    void client::process_header() {
        LOG_SCOPE
        if (_msg_buff != nullptr) free_msg_buff();
        _msg_buff_size = ntohl(*(std::int32_t*)_header_buff); // FIXME: Only linux!
        alloc_msg_buff(_msg_buff_size);
        LOG_MSG("header = " + std::to_string(_msg_buff_size))
    }
}
