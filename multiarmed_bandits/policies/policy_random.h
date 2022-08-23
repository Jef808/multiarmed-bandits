#ifndef POLICY_RANDOM_H_
#define POLICY_RANDOM_H_

#include <cassert>
#include <random>

/**
 * Select a random action.
 */
struct Policy_Random {
  template <typename Action>
  Action operator()(const std::vector<Action> &actions) const;

  std::random_device m_rd;
  mutable std::mt19937 m_gen{m_rd()};
  mutable std::uniform_int_distribution<> m_uniform;
};

template <typename Action>
Action Policy_Random::operator()(const std::vector<Action> &actions) const {
  assert(not actions.empty() && "Agent has empty actions buffer");
  std::uniform_int_distribution<> dist(0, actions.size() - 1);
  return actions[dist(m_gen)];
}

#endif // POLICY_RANDOM_H_
