#ifndef CONVERTER_H_
#define CONVERTER_H_

#include <nlohmann/json.hpp>
#include <zhelpers.hpp>

using json = nlohmann::json;

namespace nlohmann {

template <> struct adl_serializer<zmq::message_t> {
  static zmq::message_t from_json(const json &j) {
    return zmq::message_t(j.dump());
  }

  static void to_json(json &j, zmq::message_t &message) {
    j = json::parse(message.to_string_view());
  }
};

} // namespace nlohmann

#endif // CONVERTER_H_
