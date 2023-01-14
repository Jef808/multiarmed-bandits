#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#include <map>
#include <sstream>
#include <utility>
#include <variant>

#include <nlohmann/json.hpp>

namespace serialized {
struct Options {
    int nbOfSteps{1};
    int averagingWindowSize{1};
};

struct Action {
    size_t idx;
    std::string name;
    mutable std::string id;

    explicit Action(size_t idx)
        : idx{idx} { }

    std::string to_string() const {
        if (id.empty()) {
            std::ostringstream oss;
            oss << idx << '-' << name;
            id = oss.str();
        }
        return id;
    };

    bool operator==(const Action& other) const {
        return to_string() == other.to_string();
    }

    bool operator<(const Action& other) const {
        return to_string() < other.to_string();
    }
};

struct Sample {
    Action action;
    double value;

    Sample(size_t idx, double value)
        : action{idx}
        , value{value} { }
};

/**
 * `step` is the number of steps averaged into each value.
 * `action_count` is the count of each actions performed during
 * that step.
 */
struct TimePoint {
    int step;
    double value;
    std::map<std::string, int> action_count;
};

struct TimeSeries {
    std::string name;
    std::vector<TimePoint> values;
};

struct Result {
    std::string id;
    std::vector<TimeSeries> series_list;
};

namespace model {

enum Name { MultiarmedBandit = 1, Null = 0 };

struct Mab {
    int nbOfArms;
};

using Variant = std::variant<Mab>;

// NOLINTNEXTLINE(*-avoid-c-arrays)
NLOHMANN_JSON_SERIALIZE_ENUM(Name, {
                                       {            Null, "nullptr"},
                                       {MultiarmedBandit,     "mab"},
})
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Mab, nbOfArms);

} // namespace model

namespace policy {
enum Name { EpsilonGreedy = 1, Ucb = 2, Null = 0 };

struct EpsilonGreedy {
    double epsilon{0.1};
};

struct Ucb {
    double exploration{0.7};
};

using Variant = std::variant<struct EpsilonGreedy, struct Ucb>;

// NOLINTNEXTLINE(*-avoid-c-arrays)
NLOHMANN_JSON_SERIALIZE_ENUM(Name, {
                                       {         Null,       "nullptr"},
                                       {EpsilonGreedy, "epsilonGreedy"},
                                       {          Ucb,           "ucb"},
})

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(struct EpsilonGreedy, epsilon);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(struct Ucb, exploration);

} // namespace policy

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Options, nbOfSteps, averagingWindowSize);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Action, idx, name, id);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Sample, action, value);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TimePoint, step, value, action_count);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TimeSeries, name, values);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Result, id, series_list);

} // namespace serialized

#include "environments/multiarmed_bandits/multiarmed_bandits.h"
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MultiArmedBandit::Action, idx);

#endif // SERIALIZE_H_
