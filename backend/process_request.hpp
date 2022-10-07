#ifndef PROCESS_REQUEST_H_
#define PROCESS_REQUEST_H_

#include "data_models.hpp"

#include <boost/json/fwd.hpp>
#include <optional>

using parse_request_result_t = std::optional<data::request_t>;

// TODO Do string_to_enum from model/policy names to enum
// in order to process the various (has_model) and (has_policy)
// cases in two switch statements.
// Eventually should become a one-liner for models and another
// one for policies.

// TODO Separate validation and processing.
parse_request_result_t parse_request(const std::string &req) noexcept;

boost::json::object
handle_request(const parse_request_result_t &request) noexcept;

#endif // PROCESS_REQUEST_H_
