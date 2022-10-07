#ifndef AGENT_H_
#define AGENT_H_

#include "../environments/multiarmed_bandits/multiarmed_bandits.h"
#include "extactions.h"
#include "policy_random.h"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <random>
#include <utility>

template <typename Model = NArmedBandit, typename Policy = Policy_Random>
class Agent {
  using Action = typename Model::Action;
  using ExtAction = ExtActionT<Action>;

public:
  Agent(Model &bandit, Policy &policy);

  std::pair<Action, double> sample();

  /**
   * Update the agent's statistics after a
   * newly sampled action.
   */
  void update_stats(const Action &action, double reward);

  /**
   * Resets the agent's data with small gaussian noise as values.
   */
  void reset();

  /**
   * To view the running stats for each actions.
   */
  const std::vector<double> &get_action_values() const;
  const std::vector<double> &get_action_policy_values() const;
  const std::vector<double> &get_action_visits() const;

private:
  Model &m_bandit;
  std::vector<ExtAction> m_actions;
  Policy m_policy;

  mutable std::vector<double> m_visits_buffer;
  mutable std::vector<double> m_values_buffer;
  mutable std::vector<double> m_pvalues_buffer;
};

template <typename Model, typename Policy>
inline Agent<Model, Policy>::Agent(Model &bandit, Policy &policy)
    : m_bandit{bandit}, m_actions{}, m_policy{policy} {
  reset();
}

template <typename Model, typename Policy>
std::pair<typename Model::Action, double> Agent<Model, Policy>::sample() {
  typename Model::Action action = m_policy(m_actions);
  double reward = m_bandit.get_reward(action);

  update_stats(action, reward);

  return std::make_pair(action, reward);
}

template <typename Model, typename Policy>
void Agent<Model, Policy>::update_stats(const typename Model::Action &action,
                                        double reward) {
  auto it = std::find(m_actions.begin(), m_actions.end(), action);
  assert(it != m_actions.end());

  it->visits += 1;
  it->total += reward;
}

template <typename Model, typename Policy> void Agent<Model, Policy>::reset() {
  m_actions.clear();

  std::random_device rd;
  std::mt19937 gen{rd()};
  std::normal_distribution<> dist(0.0, 0.01);

  std::generate_n(std::back_inserter(m_actions), m_bandit.number_of_actions(),
                  [n = 0, &gen, &dist]() mutable {
                    ExtAction ret(n++);
                    ret.visits = 0;
                    ret.total = dist(gen);
                    return ret;
                  });

  m_policy.reset();
}

template <typename Model, typename Policy>
const std::vector<double> &Agent<Model, Policy>::get_action_values() const {
  m_values_buffer.clear();
  std::transform(m_actions.begin(), m_actions.end(),
                 std::back_inserter(m_values_buffer), [](const auto &ea) {
                   return ea.visits == 0 ? ea.total : ea.total / ea.visits;
                 });
  return m_values_buffer;
}

template <typename Model, typename Policy>
const std::vector<double> &Agent<Model, Policy>::get_action_visits() const {
  m_visits_buffer.clear();
  std::transform(m_actions.begin(), m_actions.end(),
                 std::back_inserter(m_visits_buffer),
                 [](const auto &ea) { return ea.visits; });
  return m_visits_buffer;
}

template <typename Model, typename Policy>
const std::vector<double> &
Agent<Model, Policy>::get_action_policy_values() const {
  m_pvalues_buffer.clear();
  m_policy.get_action_values(m_actions, m_pvalues_buffer);
  return m_pvalues_buffer;
}

#endif // AGENT_H_
