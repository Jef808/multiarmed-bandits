#ifndef POLICY_UCB_H_
#define POLICY_UCB_H_

#include "../extactions.h"
#include "policy_helpers.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <limits>
#include <vector>

/**
 * Use an Upper Confidence Bound formula to
 * balance exploration and exploitation.
 */
struct Policy_UCB {
  explicit Policy_UCB(double c=2.0);

  template <typename Action>
  Action operator()(const std::vector<ExtActionT<Action>> &actions) const;

  // For debugging
  template<typename Action>
  void get_action_values(const std::vector<ExtActionT<Action>>& actions,
                         std::vector<double>& ucb_values) const;

  double c;
};

inline Policy_UCB::Policy_UCB(double _c)
    : c{ _c }
{
}

template <typename Action>
double UCB(const double C, int total_visits,
           const ExtActionT<Action> &e_action) {
  if (e_action.visits == 0) {
        return std::numeric_limits<double>::max();
  }
  const double exploitation_term = SampleAverage(e_action);
  const double exploration_term = std::sqrt(std::log(total_visits / (e_action.visits)));

  return exploitation_term + C * exploration_term;
}

template <typename Action>
Action Policy_UCB::operator()(const std::vector<ExtActionT<Action>> &actions) const {
  assert(not actions.empty() && "Agent has empty actions buffer");

  int total_visits = std::accumulate(actions.begin(), actions.end(), 0,
                                     [](int& s, const auto &ea) { return s += ea.visits; });

  return *std::max_element(
      actions.begin(), actions.end(),
      [&total_visits, C=this->c](const ExtActionT<Action> &a, const ExtActionT<Action> &b) {
        return UCB(C, total_visits, a) < UCB(C, total_visits, b);
      });
}

template<typename Action>
void Policy_UCB::get_action_values(const std::vector<ExtActionT<Action>>& actions,
                                   std::vector<double>& ucb_values) const {
    int total_visits = std::accumulate(actions.begin(), actions.end(), 0,
                                     [](int& s, const auto &ea) { return s += ea.visits; });
    std::transform(actions.begin(), actions.end(), std::back_inserter(ucb_values),
                   [&total_visits, C=this->c](const auto& ea){ return UCB(C, total_visits, ea); });
}

#endif // POLICY_UCB_H_
