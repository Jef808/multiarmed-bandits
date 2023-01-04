#ifndef POLICY_UCB_H_
#define POLICY_UCB_H_

#include "extactions.h"
#include "helpers.h"

#include <algorithm>
#include <cassert>
#include <cmath> // log()
#include <iostream>
#include <limits>  // for setting result on actions with no prior visits
#include <numeric> // accumulate()
#include <vector>

namespace policy {

/**
 * Use an Upper Confidence Bound formula to
 * balance exploration and exploitation.
 */
class Ucb {
    static constexpr double kDefaultExploration = 0.7;

  public:
    explicit Ucb(double exploration = kDefaultExploration);

    /**
     * Evaluate the value of each action provided.
     */
    Action operator()(const std::vector<ExtAction>& actions) const;

    /**
     * Same as the `operator()` but save results in provided `buffer`, for
     * logging/debug purposes.
     */
    void get_action_values(const std::vector<ExtAction>& actions,
                           std::vector<double>& buffer) const;

    double& exploration() { return exploration_; }

  private:
    std::vector<double> buffer_;
    double exploration_;
};

inline Ucb::Ucb(double exploration)
    : exploration_{exploration} {
}

inline double UCB(const double explor_cst, int total_visits,
                  const ExtAction& e_action) {
    if (e_action.visits == 0) {
        return std::numeric_limits<double>::max();
    }
    const double exploitation_term = SampleAverage(e_action);
    const double exploration_term =
        explor_cst * std::sqrt(std::log(total_visits / (e_action.visits)));

    return exploitation_term + exploration_term;
}

inline Action
    Ucb::operator()(const std::vector<ExtAction>& actions) const {
    assert(not actions.empty() && "Agent has empty actions buffer");

    int total_visits =
        std::accumulate(actions.begin(), actions.end(), 0,
                        [](int&& s, const auto& ea) { return s + ea.visits; });

    return std::max_element(
        actions.begin(), actions.end(),
        [&total_visits, kExploration = this->exploration_](
            const ExtAction& a, const ExtAction& b) {
            return UCB(kExploration, total_visits, a)
                   < UCB(kExploration, total_visits, b);
        })->action;
}

inline void
    Ucb::get_action_values(const std::vector<ExtAction>& actions,
                           std::vector<double>& buffer) const {
    int total_visits =
        std::accumulate(actions.begin(), actions.end(), 0,
                        [](size_t s, const ExtAction& ea) { return s + ea.visits; });
    std::transform(
        actions.begin(), actions.end(), std::back_inserter(buffer),
        [&total_visits, kExploration = this->exploration_](const auto& ea) {
            return UCB(kExploration, total_visits, ea);
        });
}

} // namespace policy

#endif // POLICY_UCB_H_
