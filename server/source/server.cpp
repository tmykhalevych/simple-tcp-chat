#include "server.h"

namespace chat {
    server::server(boost::asio::io_service& io, const unsigned int port)
        :_port(port)
        ,_socket(io)
        ,_endpoint(tcp::v4(), _port)
        ,_acceptor(io, _endpoint)
    {}

    void server::run() {
        // TODO: Add reactions to signals

        accept();
    }

    void server::accept() {
        // TODO
    }
}
