#include "connection.h"

namespace chat {
    connection::connection(tcp::socket sock, room& rm)
        :_socket(std::move(sock))
        ,_room(rm)
    {}

    void connection::establish() const noexcept {
        // TODO
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
