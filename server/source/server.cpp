#include "server.h"
#include "connection.h"

namespace chat {
    server::server(boost::asio::io_service& io, const unsigned int port)
        :_port(port)
        ,_io(io)
        ,_socket(_io)
        ,_endpoint(tcp::v4(), _port)
        ,_acceptor(_io, _endpoint)
    {
        _acceptor.set_option(tcp::acceptor::reuse_address(true));
        accept();
    }

    void server::run() {
        _io.run();
    }

    void server::accept() {
        _acceptor.async_accept(
            _socket,
            [this](boost::system::error_code err) {
                if (!err) {
                    auto conn = std::make_shared<connection>(std::move(_socket), _room);
                    conn->establish();
                }

                accept();
            }
        );
    }
}
