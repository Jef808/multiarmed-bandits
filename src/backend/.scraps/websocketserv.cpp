#include "websocketserv.h"

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <nlohmann/json.hpp>

#include <iostream>

namespace net = boost::asio;
namespace beast = boost::beast;
using namespace boost::beast;
using namespace boost::beast::websocket;
using tcp = net::ip::tcp; // from <boost/asio/ip/tcp.hpp>
using json = nlohmann::json;

void do_session(tcp::socket socket) {
    try {
        stream<tcp::socket> ws(std::move(socket));

        // Set a decorator to change the Server of the handshake
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res) {
                res.set(http::field::server,
                        std::string(BOOST_BEAST_VERSION_STRING)
                            + " websocketserv");
            }));

        // Accept the websocket handshake
        ws.accept();

        for (;;) {
            // Store incoming messages in this buffer
            flat_buffer buffer;

            // Read incoming messages as whole messages
            ws.read(buffer);

            // Use utf8 encoded message (unless otherwise)
            ws.text(ws.got_text());

            // const char* data = rep.data();
            // const size_t sz = rep.size();
            // std::cerr << "Received\n" << data << std::endl;

            ws.write(net::const_buffer(data, sz));
            rep.clear();

            // Wrap request in a std::string
            // auto req =
            //     std::string_view{static_cast<char*>(buf_req.data().data()),
            //                      buf_req.data().size()};

            // const auto& rep = handler(req);
            // beast::flat_buffer buf_req.consume(req.size());

            // ws.write(buf_rep)
        }

    } catch (beast::system_error const& se) {
        if (se.code() != websocket::error::closed) {
            std::cerr << "Error, closed unclean" << se.code().message()
                      << std::endl;
        }
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    try {
        // Check command line arguments.
        if (argc != 3) {
            std::cerr << "Usage: websocketserv <address> <port>\n"
                      << "Example:\n"
                      << "    websocketserv 0.0.0.0 8080\n";
            return EXIT_FAILURE;
        }
        auto const address = net::ip::make_address(argv[1]);
        auto const port = static_cast<unsigned short>(std::atoi(argv[2]));

        // The io_context is required for all I/O
        net::io_context ioc{1};

        // The acceptor receives incoming connections
        tcp::acceptor acceptor{
            ioc, {address, port}
        };
        acceptor.bind(tcp::endpoint(address, port));

        // Restart on error
        for (;;) {
            // This will receive the new connection
            tcp::socket socket{ioc};

            // Block until we get a connection
            acceptor.accept(socket);

            // Launch the session, transferring ownership of the socket
            std::thread(&do_session, std::move(socket)).detach();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
