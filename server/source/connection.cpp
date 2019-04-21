#include "connection.h"
#include "comm.pb.h"
#include <sstream>

namespace chat {
    connection::connection(tcp::socket sock, room& rm)
        :_socket(std::move(sock))
        ,_room(rm)
    {
        LOG_SCOPE
    }

    connection::~connection() {
        LOG_SCOPE
        if (_msg_buff != nullptr) {
            free_msg_buff();
        }
    }

    void connection::establish() {
        LOG_SCOPE
        read_header_and(std::bind(&connection::set, this));
    }

    void connection::send(const Message& msg) {
        LOG_SCOPE
        bool not_writing = _message_q.empty();
        _message_q.push_back(msg);
        if (not_writing) {
            write();
        }
    }

    void connection::read_header_and(std::function<void(void)> next_action) {
        LOG_SCOPE
        auto self = shared_from_this();
        boost::asio::async_read(
            _socket,
            boost::asio::buffer(_header_buff, _header_buff_size),
            [this, self, next_action](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    LOG_MSG("<--- Receive <header>")
                    process_header();
                    next_action();
                }
            }
        );
    }

    void connection::process_header() {
        LOG_SCOPE
        if (_msg_buff != nullptr) free_msg_buff();
        _msg_buff_size = ntohl(*(std::int32_t*)_header_buff); // FIXME: Only linux!
        LOG_MSG("header = " + _msg_buff_size)
        alloc_msg_buff(_msg_buff_size);
    }

    void connection::set() {
        LOG_SCOPE
        auto self = shared_from_this();
        boost::asio::async_read(
            _socket,
            boost::asio::buffer(_msg_buff, _msg_buff_size),
            [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    LOG_MSG("<--- Receive <connect>. msg = " + std::string(_msg_buff, _msg_buff_size))
                    Connect conn_req;
                    std::istringstream iss(std::string(_msg_buff, _msg_buff_size));
                    conn_req.ParseFromIstream(&iss);
                    room::validation err = _room.validate(conn_req);
                    if (err == room::validation::ok) {
                        _nickname = conn_req.nickname();
                        _room.join(shared_from_this());
                        read_header_and(std::bind(&connection::read, this));
                    }
                    else {
                        send(room::get_err_msg(err));
                    }
                }
            }
        );
    }

    void connection::read() {
        LOG_SCOPE
        auto self = shared_from_this();
        boost::asio::async_read(
            _socket,
            boost::asio::buffer(_msg_buff, _msg_buff_size),
            [this, self](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    LOG_MSG("<--- Receive <message>. msg = " + std::string(_msg_buff, _msg_buff_size))
                    Message msg;
                    std::istringstream iss(std::string(_msg_buff, _msg_buff_size));
                    msg.ParseFromIstream(&iss);
                    room::validation err = _room.route(msg, get_nickname());
                    if (err != room::validation::ok) {
                        send(room::get_err_msg(err));
                    }
                }
                else if (ec != boost::asio::error::operation_aborted) {
                    LOG_MSG("Connection close...")
                    _room.kick(shared_from_this());
                }
            }
        );
    }

    void connection::write() {
        LOG_SCOPE
        auto self = shared_from_this();
        boost::asio::streambuf output_buff;
        std::ostream os(&output_buff);
        _message_q.front().SerializeToOstream(&os);
        boost::asio::async_write(
            _socket,
            output_buff,
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    _message_q.pop_front();
                    if (!_message_q.empty()) {
                        write();
                    }
                }
                else if (ec != boost::asio::error::operation_aborted) {
                    _room.kick(shared_from_this());
                }
            }
        );
    }
}
