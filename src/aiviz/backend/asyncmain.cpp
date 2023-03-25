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

class Handler {
  websocket::stream<tcp::socket> m_ws;
  beast::error_code m_erc{};
public:
  Handler(tcp::socket socket)
      : m_ws{ std::move(socket), m_erc }
  {
    m_ws.auto_fragment(true);
    init();
  }

  void init() {
    if (m_erc) {
      fail(m_erc, "Handler:ctor");
      return;
    }

    m_ws.accept(m_erc);

    on_accept();
  }

  void on_accept() {
    if (m_erc)
  }

};

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

  if (ec) {
    fail(ec, "make_address");
  }

  auto port = static_cast<net::ip::port_type>(std::atoi(argv[2]));

  tcp::endpoint endpoint = { address, port };

  int n_threads = argc == 4 ? std::atoi(argv[3]) : 1;

  // The io_context is required for all I/O
  // net::io_context ioc(n_threads);
  net::thread_pool ioc(n_threads);
  tcp::acceptor acceptor(ioc, endpoint);
  tcp::socket socket(ioc);

  for (;;) {
    acceptor.accept(ioc, endpoint, ec);

    if (ec) {
      fail(ec, "accept");
      break;
    }


  }

  return EXIT_SUCCESS;
}
