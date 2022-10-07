#include "process_request.hpp"
#include "../environments/multiarmed_bandits/multiarmed_bandits.h"
#include "../policies/agent.h"
#include "../policies/policy_greedy.h"
#include "../policies/policy_ucb.h"
#include "data.hpp"
#include "data_models.hpp"

#include <boost/json.hpp>
#include <iostream>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

template <typename> inline constexpr bool always_false_v = false;

template <typename... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
// Specify deduction guide for overloaded callables to use in std::visit
template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace json = boost::json;

parse_request_result_t parse_request(const std::string &req) noexcept {
  try {
    json::value jv = json::parse(req);
    return std::make_optional(boost::json::value_to<data::request_t>(jv));
  } catch (std::exception &e) {
    std::cerr << "parse_request: Error: " << e.what() << std::endl;
    return std::nullopt;
  }
}

namespace {

json::object handle_test_request(const data::test_request_t &request) {
  if (request.test == "dummy") {
    return json::parse(R"({"status":"success","message":"Okay"})").as_object();
  }

  if (request.test == "data") {
    return test_data_1;
  }

  else {
    std::cerr << "invalid test name: " << request.test;
    return {};
  }
}

json::object handle_data_request(const data::data_request_t &request) {

  const auto make_model = [&request] {
    return visit(
        [](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;

          if constexpr (std::is_same_v<T, data::model::mab_t>) {
            return NArmedBandit{(size_t)arg.numberOfArms};

          } else
            static_assert(always_false_v<T>, "unimplemented model type");
        },
        request.model);
  };

  const auto make_policy = [](auto &&arg) {
    using T = std::decay<decltype(arg)>;
    using result_t =
        std::conditional_t<std::is_same_v<T, data::policy::epsilon_greedy_t>,
                           Policy_Greedy, Policy_UCB>;
    return std::visit(overloaded{[](const data::policy::epsilon_greedy_t &arg) {
                                   return result_t{arg.epsilon};
                                 },
                                 [](const data::policy::ucb_t &arg) {
                                   return result_t{arg.exploration};
                                 }},
                      arg);
  };

  auto model = make_model();
  auto policy = make_policy(request.policy);

  auto agent = Agent{model, policy};
  auto parameters = request.parameters;

  std::vector<std::pair<decltype(model)::Action, double>> result;
  result.resize(parameters.numberOfSteps);

  for (int i = 0; i < parameters.numberOfSteps; ++i) {
    auto &[action, reward] = result.emplace_back();
    std::tie(action, reward) = agent.sample();
    agent.update_stats(action, reward);
  }

  return json::value_from(result).as_object();
}

} // namespace

json::object
handle_request(const std::optional<data::request_t> &request) noexcept {
  if (!request.has_value()) {
    return {};
  }

  data::request_t req = request.value();

  return std::visit(
      overloaded{
          [](data::test_request_t &arg) { return handle_test_request(arg); },
          [](data::data_request_t &arg) { return handle_data_request(arg); }},
      req.request);
}
