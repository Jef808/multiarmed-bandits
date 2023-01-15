#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace net = boost::asio;
using namespace boost::beast;
using namespace boost::beast::websocket;

// Sending messages (write, async_write sends as complete message,
// while `write_some`, `async_write_some` sends the buffer sequence
// as a partial message)
void send_buffer_sequence_one_shot(stream<tcp_stream>& ws,
                                   string_view message) {
    net::const_buffer b(message.data(), message.size());

    // Sets all outgoing messages to be sent as text.
    // (Require valid utf8 encoding)
    ws.text(true);

    ws.write(b);
}

// Receiving messages (read, async_read, read_some, async_read_some)
// Note: Can read into a `DynamicBuffer` or a `MutableBufferSequence`
void echo_message(stream<tcp_stream>& ws) {
    // DynamicBuffer
    flat_buffer buffer;

    // Read a complete message
    ws.read(buffer);

    // Set text/binary mode according to type of incoming message
    ws.text(ws.got_text());

    // Echo back the message
    ws.write(buffer.data());

    // Discard all bytes stored in the dynamic buffer
    buffer.consume(buffer.size());
}

void echo_using_partial_messages(stream<tcp_stream>& ws) {
    // DynamicBuffer
    multi_buffer buffer;

    // Read message in pieces
    do {
        ws.read_some(buffer, 512);
    } while (!ws.is_message_done());

    ws.binary(ws.got_binary());

    // Buffer adaptor allowing to iterate through buffer in pieces
    buffers_suffix<multi_buffer::const_buffers_type> cb{buffer.data()};

    // Echo in pieces
    for (;;) {
        if (buffer_bytes(cb) > 512) {
            // Specify `false` flag for "Not done"
            ws.write_some(false, buffers_prefix(512, cb));

            // Discard data from the adaptor (does not affect the underlying
            // buffer)
            cb.consume(512);
        } else {
            ws.write_some(true, cb);
            break;
        }
    }

    buffer.consume(buffer.size());
}

// Although beast takes care of replying to "pings" and "close" control messages
// by default, can register custom callbacks which will be called when
// encountering a *control frame*.
// Note: Only needs to be set once
void register_control_callback(stream<tcp_stream>& ws) {
    ws.control_callback([](frame_type kind, string_view payload) {
        // Do something
        boost::ignore_unused(kind, payload);
    });
}

// To close a session, you send a *close frame* then read and discard all
// incoming data until receiving a close frame (then shutdown and return)
// The error_code *error::closed* is correspondingly set and indicates that
// the connection has been closed cleanly.
void send_close_frame(stream<tcp_stream>& ws) {
    ws.close(close_code::normal);
}

// Note: When sending very large messages, we lose ability to respond to
// control frames in a timely manner. We can use the *automatic fragment option*
// to limit the size of outgoing messages
void set_automatic_fragment(stream<tcp_stream>& ws) {
    ws.auto_fragment(true);
    ws.write_buffer_bytes(16384);
}

// Note: Should not use the tcp_stream timeouts as they are incompatible.
// Correct way is to disable them before constructing the websocket stream
stream<tcp_stream> construct_with_timeouts(tcp_stream sock) {
    sock.expires_never();
    stream<tcp_stream> ws(std::move(sock));
    return ws;
}

// Custom Teardown (for using websocket over streams other than
// tcp::socket or net::ssl::stream as the next layer).
struct custom_stream;
void teardown(role_type role, custom_stream& stream, error_code& ec);

template <class TeardownHandler>
void async_teardown(role_type role, custom_stream& stream,
                    TeardownHandler&& handler);

// To invoke the customization point:
template <class NextLayer> struct custom_wrapper {
    NextLayer next_layer;

    template <class... Args>
    explicit custom_wrapper(Args&&... args)
        : next_layer(std::forward<Args>(args)...) { }

    friend void teardown(role_type role, custom_wrapper& stream,
                         error_code& ec) {
        // bring default implementation into scope
        using boost::beast::websocket::teardown;

        // Call customization point without namespace qualifications.
        // This allows argument-dependant lookup to take effect!
        teardown(role, stream.next_layer, ec);
    }

    template <class TeardownHandler>
    friend void async_teardown(role_type role, custom_wrapper& stream,
                               TeardownHandler&& handler) {
        using boost::beast::websocket::async_teardown;
        async_teardown(role, stream.next_layer,
                       std::forward<TeardownHandler>(handler));
    }
};

// Customizing features a-la Boost.Asio (extensible asynchronous models
// allowing for traditional completion handlers, stackful or stackless
// coroutines and futures)
void async_read_custom(stream<tcp_stream>& ws, error_code& ec) {
    flat_buffer buffer;

    ws.async_read(buffer, [](error_code, std::size_t) {
        // Do something with the buffer here
    });
}

// For example:
#ifdef USE_COROUTINE_FUTURES
#include <boost/asio/spawn.hpp>

void echo_coroutine(stream<tcp_stream>& ws, multi_buffer& buffer,
                    net::yield_context yield) {
    ws.async_read(buffer, yield);
    std::future<std::size_t> fut =
        ws.async_write(buffer.data(), net::use_future);
}
#endif

// Use a 'stream decorator' to modify behavior of streams.
template <typename Socket> void accept_only_protocols(Socket&& sock) {

    auto select_protocol = [](string_view offered_tokens) -> std::string {
        // tokenize the Sec-Websocket-Protocol header offered by client
        http::token_list offered(offered_tokens);

        static const std::array<string_view, 3> supported = {
            "v3.my.chat", "v2.my.chat", "v1.my.chat"};

        std::string result;

        for (auto proto : supported) {
            auto iter = std::find(offered.begin(), offered.end(), proto);
            if (iter != offered.end()) {
                result.assign(proto.begin(), proto.end());
                break;
            }
        }

        return result;
    };

    // Buffer for reading HTTP messages
    flat_buffer buffer;

    // Read the request
    http::request<http::string_body> req;
    http::read(sock, buffer, req);

    // Process the upgrade request
    if (websocket::is_upgrade(req)) {
        std::string protocol =
            select_protocol(req[http::field::sec_websocket_protocol]);

        if (protocol.empty()) {
            http::response<http::string_body> res;
            res.result(http::status::bad_request);
            res.body() = "No valid sub-protocol was offered.";
            http::write(sock, res);
        } else {
            // Construct the stream
            stream<tcp_stream> ws(std::forward<Socket>(sock));

            ws.set_option(stream_base::decorator(
                [protocol](http::response_header<>& hdr) {
                    hdr.set(http::field::sec_websocket_protocol, protocol);
                }));

            ws.accept(req);
        }
    } else {
        // Not a websocket upgrade request
    }
}

int main(int argc, char* argv[]) {

    // Context object
    net::io_context ioc;

    // websocket stream layered on top of a regular tcp stream
    // stream<tcp_stream> ws(ioc);

    // Note: for multithreaded usage
    // stream<tcp_stream> ws(net::make_strand(ioc));

    // Acceptor needed to accept incoming connections
    net::ip::tcp::acceptor acceptor(ioc);
    acceptor.bind(net::ip::tcp::endpoint(net::ip::tcp::v4(), 0));
    acceptor.listen();

    // accept() returns a socket which is forwarded to the tcp_stream.
    stream<tcp_stream> ws(acceptor.accept());

    // Could also directly accept the connection ourselves:
    // stream<tcp_stream> ws(net::make_strand(ioc));
    // acceptor.accept(get_lowest_layer(ws).socket());

    // Handshake (upgrading connection to a usable two-way websocket)
    const auto HTTP_WS_UPGRADE_RESPONSE = "HTTP/1.1 101 Switching Protocols"
                                          "Upgrade: websocket"
                                          "Connection: upgrade"
                                          "Sec-WebSocket-Accept: [...]"
                                          "Server: Boost.Beast";
    // accept() and async_accept() are used to perform the above automatically
    // when the upgrade request falls within the bounds of the server
    ws.accept();

    return 0;
}
