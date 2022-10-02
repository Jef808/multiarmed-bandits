#ifndef POLICY_H_
#define POLICY_H_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <random>
#include <vector>

/**
 * Structure to accumulate data along the search.
 */
template <typename Action> struct ExtActionT {
  explicit ExtActionT(size_t idx) : action{idx} {}

  Action action;
  int visits{0};
  double total{0.0};
  double prior{0.0};

  operator Action() const { return action; }
  bool operator==(const Action a) const;
};

struct Policy_UCB {
  template <typename Action>
  Action operator()(const std::vector<ExtActionT<Action>> &actions) const;

  double c{2};
};

/**
 * Select the action with the greatest average reward
 * to date.
 */
struct Policy_Greedy {
  template <typename Action>
  Action operator()(const std::vector<ExtActionT<Action>> &actions) const;
};

/**
 * Select a random action.
 */
struct Policy_Random {
  template <typename Action>
  Action operator()(const std::vector<Action> &actions) const;

  std::random_device m_rd;
  mutable std::mt19937 m_gen{m_rd()};
};

/**
 * If the action has not been visited, return the prior
 * value, otherwise the average reward it yielded to date.
 */
template <typename Action>
double SampleAverage(const ExtActionT<Action> &e_action) {
  return (e_action.visits == 0) * e_action.prior +
         (e_action.visits != 0) * (e_action.total / e_action.visits);
}

template <typename Action>
double UCB(const double c, int total_visits,
           const ExtActionT<Action> &e_action) {
  return SampleAverage(e_action) +
         c * std::sqrt(std::log(total_visits / e_action));
}

template <typename Action>
Action
Policy_UCB::operator()(const std::vector<ExtActionT<Action>> &actions) const {
  assert(not actions.empty() && "Agent has empty actions buffer");

  int total_visits = std::accumulate(actions.begin(), actions.end(), 0,
                                     [](const auto &ea) { return ea.visits; });

  return *std::max_element(
      actions.begin(), actions.end(),
      [&](const ExtActionT<Action> &a, const ExtActionT<Action> &b) {
        return UCB(c, total_visits, a) < UCB(c, total_visits, b);
      });
}

template <typename Action>
Action Policy_Greedy::operator()(
    const std::vector<ExtActionT<Action>> &actions) const {
  assert(not actions.empty() && "Agent has empty actions buffer");

  return *std::max_element(
      actions.begin(), actions.end(),
      [](const ExtActionT<Action> &a, const ExtActionT<Action> &b) {
        return SampleAverage(a) < SampleAverage(b);
      });
}

template <typename Action>
Action Policy_Random::operator()(const std::vector<Action> &actions) const {
  assert(not actions.empty() && "Agent has empty actions buffer");
  std::uniform_int_distribution<> dist(0, actions.size() - 1);
  return actions[dist(m_gen)];
}

#endif // POLICY_H_
