#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "aiviz/backend/request_handler.h"
#include "aiviz/environments/multiarmed_bandits/multiarmed_bandits.h"
#include "aiviz/policies/agent.h"
#include "aiviz/policies/policies.h"

#include <cassert>
#include <cstddef>
#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

template <typename ErrorCode> void fail(ErrorCode ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

void set_config(websocket::stream<tcp::socket>& ws) {
    // To be called on the next read after receiving a control frame.
    ws.control_callback(
        [&ws](websocket::frame_type type, beast::string_view payload) {
            using websocket::frame_type;
            const char* str = type == frame_type::close  ? "CLOSE"
                              : type == frame_type::ping ? "PING"
                              : type == frame_type::pong ? "PONG"
                                                         : "";
            std::cout << "** Control " << str << " **";
            if (!ws.reason().reason.empty())
                std::cout << " : " << ws.reason().reason;
            std::cout << std::endl;
        });

    // Set suggested timeout settings for the websocket
    ws.set_option(
        websocket::stream_base::timeout::suggested(beast::role_type::server));

    // Set some header fields for the lifetime of the websocket
    ws.set_option(
        websocket::stream_base::decorator([](websocket::response_type& res) {
            res.set(http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) + " serverbackend");
        }));
}

struct RequestHandlerErrorCode : public std::error_code {
    [[nodiscard]] const char* message() const noexcept { return "...Error..."; }
};

void run_sync(tcp::socket socket) {
    using beast::error_code;
    using beast::tcp_stream;
    using websocket::stream;

    // Container to save work requests that accumulates
    // Queue queue;

    error_code ec;

    Query::RequestHandler request_handler;

    websocket::stream<tcp::socket> ws{std::move(socket)};

    try {
        // Setup the websocket config
        set_config(ws);

        // Accept the websocket handshake
        ws.accept(ec);
        if (ec) {
            return fail(ec, "accept");
        }

        // std::size_t bytes_read = 0;

        // The buffer where we temporarily store the incoming requests.
        beast::flat_buffer buffer;

        // Buffer for outcoming responses.
        beast::flat_buffer buffer_rep;
        std::string        rep;

        bool okay = true;

        for (;;) {
            // Read request
            ws.read(buffer, ec);

            if (ec == websocket::error::closed) {
                return;
            }
            if (ec) {
                return fail(ec, "read");
            }

            // Set websocket to text mode (if text was read)
            ws.text(ws.got_text());

            // We expect messages to be delivered in one frame.
            assert(ws.is_message_done());  // NOLINT

            // queue.push(
            //     static_cast<const char*>(buffer.data().data()), buffer.size());
            // std::tie(okay, rep) = request_handler(queue.pop());

            // Appends all the writeable bytes on top of the readable bytes
            // buffer.commit(buffer.max_size());
            // req = std::string(static_cast<const char*>(buffer.data().data()), buffer.size());

            std::tie(okay, rep) = request_handler(beast::buffers_to_string(buffer.data()));

            if (not okay) {
                return fail(RequestHandlerErrorCode{}, "request_handler");
            }

            // buffer_rep.data() = ;
            // size_t n = net::buffer_copy(buffer_rep.prepare(rep.size()),

            // net::buffer(rep));
            // buffer_rep.commit(n);
            // buffer_rep.prepare(rep.size()) = { rep.data(), rep.size() };
            // buffer_rep.commit(rep.size());

            ws.write(net::buffer(rep), ec);

            if (ec) {
                return fail(ec, "write");
            }

            // // Clear the buffer
            buffer.consume(buffer.size());
            buffer_rep.consume(buffer_rep.size());
        }

    } catch (beast::system_error const& se) {
        if (se.code() == websocket::error::closed) {
          std::string msg = "ERROR! websocket::error::closed was caught instead causing graceful termination";
          throw std::runtime_error(msg);
        }

        std::cerr << "Server error: " << se.code().message()
                  << std::endl;

    } catch (std::exception const& e) {
        std::cerr << "Unexpected server error: " << e.what()
                  << std::endl;
    }
}

void run_forever(std::string_view address, int port, ) {

}

int main(int argc, char* argv[]) {

    if (argc < 3) {
      std::cerr << "Usage: serverbackend <address> <port> [threads]\n"
                << "Example:\n"
                << "    serverbackend 0.0.0.0 8080 [2]\n";
      return EXIT_FAILURE;
    }

    beast::error_code ec;

    // Get tcp socket endpoint from command line arguments
    auto address = net::ip::make_address(argv[1], ec);
    auto port = static_cast<net::ip::port_type>(std::atoi(argv[2]));
    tcp::endpoint endpoint = { address, port };

    int n_threads = 1;

    if (argc == 4) {
      n_threads = std::atoi(argv[3]);
    }

    // The io_context is required for all I/O
    net::io_context ioc(n_threads);

    try {
        // Listen for incoming connections
        tcp::acceptor acceptor(ioc, endpoint);
        std::cout << "Websocket accepting connections at "
                    << address << ": " << port << std::endl;

        for (;;) {

          net::socket_base::linger linger(true, 10);
          tcp::socket socket{ioc};
          socket.set_option(linger);

            // Block until we get a connection
            acceptor.accept(socket, ec);
            if (ec) {
                fail(ec, "accept");
                break;
            }

            std::cout << "Got connection, proceeding with websocket handshake"
                      << std::endl;

            // Launch the session
            run_sync(std::move(socket));

                // std::thread(&run_sync, std::move(socket)).detach();
        }
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error during initialization: " << e.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
