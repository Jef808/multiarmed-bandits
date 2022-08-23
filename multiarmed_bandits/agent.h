#ifndef AGENT_H_
#define AGENT_H_

#include "multiarmed_bandits.h"
#include "extactions.h"

#include <numeric>
#include <algorithm>
#include <cassert>
#include <random>

template <typename Policy> class Agent {
  using ExtAction = ExtActionT<Action>;

public:
  Agent(NArmedBandit &bandit, Policy &&policy);

  std::pair<Action, double> sample();

  /**
   * Select the action using the greedy policy with
   * probability 1-epsilon, and select a random action
   * with probability epsilon.
   */
  Action select();

  /**
   * Update the agent's statistics after a
   * newly sampled action.
   */
  void update_stats(const Action &action, double reward);

  /**
   * Resets the agent's data with small gaussian noise as values.
   */
  void reset();

  /// For debugging
  void get_action_values(std::vector<double>& values) const;
  void get_action_policy_values(std::vector<double>& pvalues) const;
  void get_action_visits(std::vector<int>& visits) const;

private:
  NArmedBandit &m_bandit;
  std::vector<ExtAction> m_actions;

  Policy m_policy;
  double m_initial_estimate{0.0};
};

template <typename Policy>
inline Agent<Policy>::Agent(NArmedBandit &bandit, Policy &&policy)
    : m_bandit{bandit}, m_actions{}, m_policy{policy} {
    reset();
}

template <typename Policy> std::pair<Action, double> Agent<Policy>::sample() {
  Action action = m_policy(m_actions);
  double reward = m_bandit.get_reward(action);



  update_stats(action, reward);

  return std::make_pair(action, reward);
}

template <typename Policy>
void Agent<Policy>::update_stats(const Action &action, double reward) {
  auto it = std::find(m_actions.begin(), m_actions.end(), action);
  assert(it != m_actions.end());

  it->visits += 1;
  it->total += reward;
}

template <typename Policy> void Agent<Policy>::reset() {
  m_actions.clear();

  std::random_device rd;
  std::mt19937 gen{rd()};
  std::normal_distribution<> dist(0.0, 0.01);

  std::generate_n(std::back_inserter(m_actions), m_bandit.number_of_actions(),
                  [n=0, &gen, &dist]() mutable {
                      ExtAction ret(n++);
                      ret.visits = 0;
                      ret.total = dist(gen);
                      return ret;
                  });
}

template <typename Policy>
void Agent< Policy >::get_action_values(std::vector<double>& values) const {
    std::transform(m_actions.begin(), m_actions.end(), std::back_inserter(values),
                   [](const auto& ea){ return ea.visits==0? ea.total: ea.total/ea.visits; });
}

template <typename Policy>
void Agent< Policy >::get_action_policy_values(std::vector<double>& pvalues) const {
    m_policy.get_action_values(m_actions, pvalues);
}

template<typename Policy>
void Agent<Policy>::get_action_visits(std::vector<int>& visits) const {
    std::transform(m_actions.begin(), m_actions.end(), std::back_inserter(visits),
                   [](const auto& ea){ return ea.visits; });
}

#endif // AGENT_H_
