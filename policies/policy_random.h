#ifndef POLICY_RANDOM_H_
#define POLICY_RANDOM_H_

#include <cassert>
#include <random>

/**
 * Select a random action.
 */
struct Policy_Random {
  Policy_Random() = default;

  template <typename Action>
  Action operator()(const std::vector<Action> &actions) const;

  void reset();

  static inline std::random_device rd{};
  static inline std::mt19937 m_gen{ rd() };
};

template <typename Action>
Action Policy_Random::operator()(const std::vector<Action> &actions) const {
  assert(not actions.empty() && "Agent has empty actions buffer");
  std::uniform_int_distribution<> dist(0, actions.size() - 1);
  return actions[dist(m_gen)];
}

void Policy_Random::reset() {
    m_gen.seed(rd());
}

#endif // POLICY_RANDOM_H_
