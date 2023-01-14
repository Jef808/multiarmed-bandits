#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "aiviz/backend/request_handler.h"
#include "aiviz/environments/multiarmed_bandits/multiarmed_bandits.h"
#include "aiviz/policies/policies.h"
#include "aiviz/policies/agent.h"

#include <atomic>
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

template<typename ErrorCode>
void fail(ErrorCode ec, char const* what) {
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


struct Queue {

    // Push at back, return const ref to first one item
    const std::string& push(std::string&& s) {
        data.push_back(std::move(s));
        return data.back();
    }
    // Overload emplacing back the string from a const pointer
    // to contiguous chars
    const std::string& push(const char* ps, std::size_t sz) {
        return data.emplace_back(ps, sz);
    }

    // Get the first item in the queue (removing it from queue)
    std::string pop() {
      std::string ret = data.front();
      data.pop_front();
      return ret;
    }

    std::mutex m;
    std::deque<std::string> data;
};

struct RequestHandlerErrorCode : std::error_code {
  const char* message() const noexcept { return "...Error..."; }
};


void run(tcp::socket socket) {

    using beast::error_code;
    using beast::tcp_stream;
    using websocket::stream;

    // Container to save any work that accumulates
    Queue queue;
    error_code ec;

    Query::RequestHandler request_handler;

    try {
        websocket::stream<tcp::socket> ws{std::move(socket)};

        // Setup the websocket config
        set_config(ws);

        // Accept the websocket handshake
        ws.accept(ec);
        if (ec)
            return fail(ec, "accept");

        std::size_t bytes_read = 0;

        // The buffer where we temporarily store the incoming requests.
        beast::flat_buffer buffer;
        beast::flat_buffer buffer_rep;

        bool okay = true;
        std::string rep;

        for (;;) {

            // Read request
            std::size_t bytes_transmitted = ws.read(buffer, ec);
            if (ec == websocket::error::closed)
                return;
            if (ec)
                return fail(ec, "read");

            // Set websocket to text mode (if text was read)
            ws.text(ws.got_text());

            // In this implementation, we expect messages to be delivered
            // in one frame.
            assert(ws.is_message_done());

            const std::string& req = queue.push(static_cast<const char*>(buffer.data().data()), buffer.size());
            std::tie(okay, rep) = request_handler(queue.pop());

            if (not okay) {
              return fail(RequestHandlerErrorCode{}, "request_handler");
            }

            size_t n = net::buffer_copy(buffer_rep.prepare(rep.size()), net::buffer(rep));
            buffer_rep.commit(n);
            ws.write(buffer_rep.data(), ec);
            if (ec)
                return fail(ec, "write");

            // // Clear the buffer
            buffer.consume(buffer.size());
            buffer_rep.consume(buffer_rep.size());
        }

    } catch (beast::system_error const& se) {
        // Session was closed
        if (se.code() != websocket::error::closed) {
            std::cerr << "Unexpected server error: " << se.code().message()
                      << std::endl;
        }
    } catch (std::exception const& e) {
        std::cerr << "Unexpected implementation error: " << e.what()
                  << std::endl;
    }
}

int main(int argc, char* argv[]) {

    beast::error_code ec;

    try {
        // Parse command line arguments.
        if (argc != 3) {
            std::cerr << "Usage: serverbackend <address> <port>\n"
                      << "Example:\n"
                      << "    serverbackend 0.0.0.0 8080\n";
            return EXIT_FAILURE;
        }

        auto address = net::ip::make_address(argv[1]);
        auto port = static_cast<unsigned short>(std::atoi(argv[2]));

        // The io_context is required for all I/O
        net::io_context ioc;

        // Listen for incoming connections
        tcp::acceptor acceptor(ioc, {address, port});

        for (;;) {

            tcp::socket socket{ioc};

            std::cout << "Websocket accepting connections at " << address
                      << ": " << port << std::endl;

            // Block until we get a connection
            acceptor.accept(socket, ec);
            if (ec) {
                fail(ec, "accept");
                break;
            }

            std::cout << "Got connection, proceeding with websocket handshake"
                      << std::endl;

            // Launch the session
            std::thread(&run, std::move(socket)).detach();
        }
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error during initialization: " << e.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
