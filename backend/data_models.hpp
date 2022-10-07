#ifndef DATA_MODELS_H_
#define DATA_MODELS_H_

//#include <concepts>
#include "../environments/multiarmed_bandits/multiarmed_bandits.h"
#include <nlohmann/json.hpp>
//#include "data_models_helpers.hpp"
//#include <boost/json.hpp>
//#include <set>
#include <string>

//#include <tuple>
//#include <unordered_map>

template <typename E, typename I = std::underlying_type_t<E>> I to_int(E e) {
    return static_cast<I>(e);
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NArmedBandit::Action, idx);

namespace data {

enum Category { Model, Policy, Null = -1 };

NLOHMANN_JSON_SERIALIZE_ENUM(Category, {
                                           {  Null, "nullptr"},
                                           { Model,   "model"},
                                           {Policy,  "policy"},
})

//////////////////////////////
// Description of models
//////////////////////////////
namespace model {

enum Name { N_Mab, N_Null = -1 };

NLOHMANN_JSON_SERIALIZE_ENUM(Name, {
                                       {N_Null, "nullptr"},
                                       { N_Mab,     "mab"},
})

struct mab_t {
    int numberOfArms;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(mab_t, numberOfArms);

} // namespace model

//////////////////////////////
// Description of policies
//////////////////////////////
namespace policy {

enum Name { N_EpsilonGreedy, N_Ucb, N_Null = -1 };

NLOHMANN_JSON_SERIALIZE_ENUM(Name, {
                                       {         N_Null,       "nullptr"},
                                       {N_EpsilonGreedy, "epsilonGreedy"},
                                       {          N_Ucb,           "ucb"},
})

struct epsilon_greedy_t {
    double epsilon;
};

struct ucb_t {
    double exploration;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(epsilon_greedy_t, epsilon);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ucb_t, exploration);

} // namespace policy

//////////////////////////////
// Description of request info
//////////////////////////////
struct parameters_t {
    int numberOfSteps;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(parameters_t, numberOfSteps);

/////////////////////////////////
// Description of response type
/////////////////////////////////
template <typename A> struct sample_result_t {
    std::string action;
    double value{};

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(sample_result_t<A>, action, value);
};

template <typename A> using result_t = std::vector<sample_result_t<A>>;

} // namespace data

template <typename A>
data::sample_result_t<A> wrap_sample(const A& a, double v) {
    return {a.to_string(), v};
}

#endif // DATA_MODELS_H_
