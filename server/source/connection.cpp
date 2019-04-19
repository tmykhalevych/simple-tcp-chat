#include "connection.h"
#include "comm.pb.h"
#include <sstream>

namespace chat {
    connection::connection(tcp::socket sock, room& rm)
        :_socket(std::move(sock))
        ,_room(rm)
    {
        _input_buff.fill(0);
    }

    void connection::establish() {
        auto self = shared_from_this();
        boost::asio::async_read(
            _socket,
            boost::asio::buffer(_input_buff, input_buff_dim),
            [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    Connect conn_req;
                    std::istringstream iss(std::string(_input_buff.begin(), _input_buff.begin() + bytes_transferred));
                    conn_req.ParseFromIstream(&iss);
                    room::validation err = _room.validate(conn_req);
                    if (err == room::validation::ok) {
                        _nickname = conn_req.nickname();
                        _room.join(shared_from_this());
                        _input_buff.fill(0);
                        read();
                    }
                    else {
                        send(room::get_err_msg(err));
                    }
                }
            }
        );
    }

    void connection::send(const Message& msg) {
        bool not_writing = _message_q.empty();
        _message_q.push_back(msg);
        if (not_writing) {
            write();
        }
    }

    void connection::read() {
        auto self = shared_from_this();
        boost::asio::async_read(
            _socket,
            boost::asio::buffer(_input_buff, input_buff_dim),
            [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    Message msg;
                    std::istringstream iss(std::string(_input_buff.begin(), _input_buff.begin() + bytes_transferred));
                    msg.ParseFromIstream(&iss);
                    room::validation err = _room.route(msg, get_nickname());
                    if (err != room::validation::ok) {
                        send(room::get_err_msg(err));
                    }
                    _input_buff.fill(0);
                }
                else if (ec != boost::asio::error::operation_aborted) {
                    _room.kick(shared_from_this());
                }
            }
        );
    }

    void connection::write() {
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
