#ifndef POLICY_GREEDY_H_
#define POLICY_GREEDY_H_

#include "extactions.h"
#include "policies/helpers.h"
#include "policies/random.h"

#include <algorithm>
#include <cassert>
#include <random>
#include <vector>

namespace policy {

/**
 * Select the action with the greatest average reward
 * to date.
 */
struct EpsilonGreedy {

    explicit EpsilonGreedy(double epsilon,
                           uint32_t seed = std::random_device{}());

    template <typename Action>
    Action operator()(const std::vector<ExtAction<Action>>& actions);

    void seed(uint32_t seed = std::mt19937::default_seed);

    double epsilon;
    Random policy_random;
};

inline EpsilonGreedy::EpsilonGreedy(double _epsilon, uint32_t seed)
    : epsilon{_epsilon}
    , policy_random{seed} {
}

template <typename Action>
Action
    EpsilonGreedy::operator()(const std::vector<ExtAction<Action>>& actions) {
    assert(not actions.empty() && "Agent has empty actions buffer");

    std::bernoulli_distribution m_bernoulli(epsilon);
    const bool chance = m_bernoulli(policy_random.m_gen);

    if (chance)
        return policy_random(actions);

    return *std::max_element(
        actions.begin(), actions.end(),
        [](const ExtAction<Action>& a, const ExtAction<Action>& b) {
            return SampleAverage(a) < SampleAverage(b);
        });
}

void EpsilonGreedy::seed(uint32_t seed) {

    policy_random.seed(seed);
}

} // namespace policy

#endif // POLICY_GREEDY_H_
