#ifndef POLICY_GREEDY_H_
#define POLICY_GREEDY_H_

#include "../extactions.h"
#include "policy_helpers.h"
#include "policy_random.h"

#include <algorithm>
#include <cassert>
#include <random>
#include <vector>

/**
 * Select the action with the greatest average reward
 * to date.
 */
struct Policy_Greedy {
  explicit Policy_Greedy(double epsilon = 0.0);

  template <typename Action>
  Action operator()(const std::vector<ExtActionT<Action>> &actions) const;

  double epsilon;
  Policy_Random policy_random;
  mutable std::bernoulli_distribution m_bernoulli;
};

inline Policy_Greedy::Policy_Greedy(double _epsilon)
    : epsilon{_epsilon}, policy_random{}, m_bernoulli{_epsilon} {}

template <typename Action>
Action Policy_Greedy::operator()(
    const std::vector<ExtActionT<Action>> &actions) const {
  assert(not actions.empty() && "Agent has empty actions buffer");

  const bool chance = m_bernoulli(policy_random.m_gen);

  if (chance) {
    return policy_random(actions);
  }

  return *std::max_element(
      actions.begin(), actions.end(),
      [](const ExtActionT<Action> &a, const ExtActionT<Action> &b) {
        return SampleAverage(a) < SampleAverage(b);
      });
}

#endif // POLICY_GREEDY_H_
