#include "connection.h"

namespace chat {
    connection::connection(tcp::socket sock, room& rm)
        :_socket(std::move(sock))
        ,_room(rm)
    {}

    void connection::establish() {
        room::validation err = _room.validate(shared_from_this());
        if (err == room::validation::ok) {
            _room.join(shared_from_this());
            read();
        }
        else {
            send(room::get_err_msg(err));
        }
    }

    void connection::send(const message& msg) {
        // TODO
    }

    void connection::read() {
        // TODO
    }

    void write() {
        // TODO
    }
}
