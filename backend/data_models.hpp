#ifndef DATA_MODELS_H_
#define DATA_MODELS_H_

#include "data_models_helpers.hpp"
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>

template <typename E, typename I = std::underlying_type_t<E>> I to_int(E e) {
  return static_cast<I>(e);
}

namespace data {

//////////////////////////////
// Description of models
//////////////////////////////
namespace model {

enum names { mab = 1 };

BOOST_DESCRIBE_ENUM(names, mab);

struct mab_t {
  int numberOfArms;
};

BOOST_DESCRIBE_STRUCT(mab_t, (), (numberOfArms));

} // namespace model

//////////////////////////////
// Description of policies
//////////////////////////////
namespace policy {

struct epsilon_greedy_t {
  double epsilon;
};

struct ucb_t {
  double exploration;
};

BOOST_DESCRIBE_STRUCT(epsilon_greedy_t, (), (epsilon));
BOOST_DESCRIBE_STRUCT(ucb_t, (), (exploration));

} // namespace policy

//////////////////////////////
// Description of request info
//////////////////////////////
struct parameters_t {
  int numberOfSteps;
};

BOOST_DESCRIBE_STRUCT(parameters_t, (), (numberOfSteps));

using model_t = std::variant<model::mab_t>;
using policy_t = std::variant<policy::epsilon_greedy_t, policy::ucb_t>;

struct data_request_t {
  model_t model;
  policy_t policy;
  parameters_t parameters;
};

struct test_request_t {
  std::string test;
};

struct request_t {
  std::variant<data_request_t, test_request_t> request;
};

BOOST_DESCRIBE_STRUCT(data_request_t, (), (model, policy, parameters));
BOOST_DESCRIBE_STRUCT(test_request_t, (), (test));
BOOST_DESCRIBE_STRUCT(request_t, (), (request));
} // namespace data

/**
 * Usage:
 *
 * boost::json::value jv{{"numberOfArms", 10}};
 * auto mab = boost::json::value_to<data::model::mab_t>(jv);
 *
 * etc...
 */

#endif // DATA_MODELS_H_
