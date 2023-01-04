#ifndef POLICY_GREEDY_H_
#define POLICY_GREEDY_H_

#include "extactions.h"
#include "helpers.h"
#include "random.h"

#include <algorithm>
#include <cassert>
#include <random>
#include <vector>

namespace policy {

/**
 * Select the action with the greatest average reward
 * to date.
 */
class EpsilonGreedy {
    static constexpr double defaultEpsilon = 0.1;

  public:
    explicit EpsilonGreedy(double epsilon = defaultEpsilon,
                           uint32_t seed = std::random_device{}());

    Action operator()(const std::vector<ExtAction>& actions);

    void seed(uint32_t seed = std::mt19937::default_seed);

    double& epsilon() { return m_epsilon; }

    private:
    double m_epsilon;
    Random policy_random;
};

inline EpsilonGreedy::EpsilonGreedy(double _epsilon, uint32_t seed)
    : m_epsilon{_epsilon}
    , policy_random{seed} {
}

Action
    EpsilonGreedy::operator()(const std::vector<ExtAction>& actions) {
    assert(not actions.empty() && "Agent has empty actions buffer");

    std::bernoulli_distribution m_bernoulli(m_epsilon);
    const bool chance = m_bernoulli(policy_random.m_gen);

    if (chance)
        return policy_random(actions).action;

    return std::max_element(
        actions.begin(), actions.end(),
        [](const ExtAction& a, const ExtAction& b) {
            return SampleAverage(a) < SampleAverage(b);
        })->action;
}

inline void EpsilonGreedy::seed(uint32_t seed) {

    policy_random.seed(seed);
}

} // namespace policy

#endif // POLICY_GREEDY_H_
