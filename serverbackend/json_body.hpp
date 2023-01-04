#ifndef JSON_BODY_H_
#define JSON_BODY_H_

#include <boost/asio/buffer.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/json.hpp>
#include <boost/json/monotonic_resource.hpp>
#include <boost/json/stream_parser.hpp>

#include <boost/json/src.hpp>

#include <iostream>
#include <optional>
#include <string>

namespace API {

struct JsonBody {
    using value_type = boost::json::value;

    struct writer {
        using const_buffers_type = boost::asio::const_buffer;

        template <bool isRequest, class Fields>
        writer(boost::beast::http::header<isRequest, Fields> const&,
               value_type const& body) {
            serializer.reset(&body);
        }

        void init(boost::system::error_code& ec) { ec = {}; }

        boost::optional<std::pair<const_buffers_type, bool>>
            get(boost::system::error_code& ec) {
            ec = {};
            // Serialize as much as possible with the buffer.
            const auto len = serializer.read(buffer.data(), sizeof(buffer));
            return std::make_pair(
                boost::asio::const_buffer(len.data(), len.size()),
                !serializer.done());
        }

      private:
        boost::json::serializer serializer;
        std::array<char, 32768> buffer{};
    };

    struct reader {
        template <bool isRequest, class Fields>
        reader(boost::beast::http::header<isRequest, Fields>&, value_type& body)
            : body(body) { }

        void init(boost::optional<std::uint64_t> const& content_length,
                  boost::system::error_code& ec) {
            if (content_length)
                parser.reset(boost::json::make_shared_resource<
                             boost::json::monotonic_resource>(*content_length));
            ec = {};
        }

        template <class ConstBufferSequence>
        std::size_t put(ConstBufferSequence const& buffers,
                        boost::system::error_code& ec) {
            ec = {};
            return parser.write_some(static_cast<const char*>(buffers.data()),
                                     buffers.size(), ec);
        }

        void finish(boost::system::error_code& ec) {
            ec = {};
            // Check if json is complete
            if (parser.done())
                body = parser.release();
            else
                ec = boost::json::error::incomplete;
        }

        // Note: Optionally, can implement
        // static
        // std::uint64_t
        // size(value_type const& body);

      private:
        boost::json::stream_parser parser;
        value_type& body;
    };
};

static_assert(boost::beast::http::is_body<JsonBody>::value);

} // namespace API

#endif // JSON_BODY_H_
