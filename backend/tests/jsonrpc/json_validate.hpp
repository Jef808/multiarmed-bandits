#ifndef JSON_VALIDATE_H_
#define JSON_VALIDATE_H_

#include <boost/json.hpp>
#include <boost/json/src.hpp>

// This file must be manually included when
// using basic_parser to implement a parser.
#include <boost/json/basic_parser_impl.hpp>

#include <iomanip>
#include <iostream>

using namespace boost::json;

namespace rpc {

// The null parser discards all the data
class null_parser {
  struct handler {
    constexpr static std::size_t max_object_size = std::size_t(-1);
    constexpr static std::size_t max_array_size = std::size_t(-1);
    constexpr static std::size_t max_key_size = std::size_t(-1);
    constexpr static std::size_t max_string_size = std::size_t(-1);

    bool on_document_begin(error_code &) { return true; }
    bool on_document_end(error_code &) { return true; }
    bool on_object_begin(error_code &) { return true; }
    bool on_object_end(std::size_t, error_code &) { return true; }
    bool on_array_begin(error_code &) { return true; }
    bool on_array_end(std::size_t, error_code &) { return true; }
    bool on_key_part(string_view, std::size_t, error_code &) { return true; }
    bool on_key(string_view, std::size_t, error_code &) { return true; }
    bool on_string_part(string_view, std::size_t, error_code &) { return true; }
    bool on_string(string_view, std::size_t, error_code &) { return true; }
    bool on_number_part(string_view, error_code &) { return true; }
    bool on_int64(std::int64_t, string_view, error_code &) { return true; }
    bool on_uint64(std::uint64_t, string_view, error_code &) { return true; }
    bool on_double(double, string_view, error_code &) { return true; }
    bool on_bool(bool, error_code &) { return true; }
    bool on_null(error_code &) { return true; }
    bool on_comment_part(string_view, error_code &) { return true; }
    bool on_comment(string_view, error_code &) { return true; }
  };

  basic_parser<handler> p_;

public:
  null_parser() : p_(parse_options()) {}

  ~null_parser() {}

  std::size_t write(char const *data, std::size_t size, error_code &ec) {
    auto const n = p_.write_some(false, data, size, ec);
    if (!ec && n < size)
      ec = error::extra_data;
    return n;
  }
};

inline bool validate(string_view s) {
  // Parse with the null parser and return false on error
  null_parser p;
  error_code ec;
  p.write(s.data(), s.size(), ec);
  if (ec)
    return false;

  // The string is valid JSON.
  return true;
}

} // namespace rpc

#endif // JSON_VALIDATE_H_
