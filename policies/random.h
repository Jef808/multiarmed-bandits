#ifndef POLICY_RANDOM_H_
#define POLICY_RANDOM_H_

#include <cassert>
#include <random>

namespace policy {

/**
 * Select a random action.
 */
struct Random {
    Random(uint32_t q = std::random_device{}())
        : m_gen{q} { }

    template<typename Action>
    Action operator()(const std::vector<Action>& actions);

    void seed(uint32_t q = std::mt19937::default_seed);

    std::mt19937 m_gen;
};

template <typename Action>
Action Random::operator()(const std::vector<Action>& actions) {
    assert(not actions.empty() && "Agent has empty actions buffer");
    std::uniform_int_distribution<> dist(0, actions.size() - 1);
    return actions[dist(m_gen)];
}

inline void Random::seed(uint32_t q) {
    m_gen.seed(q);
}

} // namespace policy

#endif // POLICY_RANDOM_H_
