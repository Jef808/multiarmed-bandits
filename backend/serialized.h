#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#include <utility>
#include <variant>

#include <nlohmann/json.hpp>

namespace serialized {
struct parameters {
    int numberOfSteps;
};

struct sample {
    std::string action;
    double value;

    sample(std::string&& action_, double value_)
        : action{std::move(action_)}
        , value{value_} { }
};

namespace model {

struct Mab {
    int numberOfArms;
};

} // namespace model

namespace policy {
struct EpsilonGreedy {
    double epsilon;
};

struct Ucb {
    double exploration;
};

} // namespace policy

namespace model {
enum Name { kMab = 0, kNull = -1 };

using Variant = std::variant<Mab>;

// NOLINTNEXTLINE(*-avoid-c-arrays)
NLOHMANN_JSON_SERIALIZE_ENUM(Name, {
                                       {kNull, "nullptr"},
                                       { kMab,     "mab"},
})
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Mab, numberOfArms);

} // namespace model

namespace policy {
enum Name { kEpsilonGreedy = 0, kUcb = 1, kNull = -1 };

using Variant = std::variant<EpsilonGreedy, Ucb>;

// NOLINTNEXTLINE(*-avoid-c-arrays)
NLOHMANN_JSON_SERIALIZE_ENUM(Name, {
                                       {         kNull,       "nullptr"},
                                       {kEpsilonGreedy, "epsilonGreedy"},
                                       {          kUcb,           "ucb"},
})

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EpsilonGreedy, epsilon);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Ucb, exploration);

} // namespace policy

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(parameters, numberOfSteps);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(sample, action, value);

} // namespace serialized

#include "../environments/multiarmed_bandits/multiarmed_bandits.h"
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MultiArmedBandit::Action, idx);

#endif // SERIALIZE_H_
