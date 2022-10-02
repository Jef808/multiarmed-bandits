#ifndef JSON_RECV_H_
#define JSON_RECV_H_

#include "json_validate.hpp"

#include <boost/json.hpp>
#include <boost/json/impl/error.hpp>
#include <boost/json/value_ref.hpp>

#include <tests/zhelpers.hpp>
#include <zmq.hpp>
#include <zmq_addon.hpp>

#include <optional>
#include <vector>

namespace json = boost::json;
namespace rpc {

class json_recv_t {
public:
  json_recv_t() = default;

  [[nodiscard]] std::optional<json::value> recv(zmq::socket_ref socket) {

    try {
      zmq::recv_result_t result = mmsg_.recv(socket); // can throw

      if (result) {
        encode(mmsg_).move(msg_); // can throw

        if (auto s_msg = msg_.to_string_view(); validate(s_msg)) {
          return std::move(json::parse(s_msg)); // should not throw but can!
        }
      }

    } catch (zmq::error_t &e) {
      std::cerr << "json_recv_t:recv -- \033[1mzmq_error:\033[0m" << e.what()
                << std::endl;
    }

    return std::make_optional(std::move(jv_));
  }

private:
  zmq::multipart_t mmsg_;
  zmq::message_t msg_;
  json::value jv_;
};

} // namespace rpc

#endif // JSON_RECV_H_
