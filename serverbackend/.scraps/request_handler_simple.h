#ifndef REQUEST_HANDLER_SIMPLE_H_
#define REQUEST_HANDLER_SIMPLE_H_

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/websocket.hpp>

#include "json_body.hpp"

#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace API {

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;

struct Task { };

class RequestHandler {
    using tcp = net::ip::tcp;

  public:
    RequestHandler(tcp::socket socket)
        : ws{std::move(socket)} { }

    void init() { }

    void read() { }

    void run() {
        for (;;) {
            // Read a message into the persistent buffer
            ws.read(buffer);

            // Register the work on the queue.
            queue.push_back()
        }
    }

  private:
    websocket::stream<tcp::socket> ws;
    beast::flat_buffer buffer;
    std::vector<Task> queue;
};

} // namespace API

#endif // REQUEST_HANDLER_SIMPLE_H_
