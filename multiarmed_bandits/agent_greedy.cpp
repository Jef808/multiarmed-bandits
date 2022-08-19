/**
 * Let ${a_1, \ldots, a_n}$ be the possible actions which
 * can be taken at each time step.
 *
 * We denote by $q(a)$ the expected reward from choosing
 * action $a$,
 * and by $Q_t(a)$ the estimated value on the $t$th time step.
 */
#include "agent_greedy.h"
#include "multiarmed_bandits.h"

#include <algorithm>
#include <cassert>
#include <random>

AgentGreedy::AgentGreedy(NArmedBandit &bandit, double epsilon)
    : m_bandit{bandit}, m_actions{},
      m_epsilon{epsilon},
      m_uniform(0, bandit.number_of_actions() - 1) {

}

void AgentGreedy::sample() {
  Action action = select();
  double reward = m_bandit.get_reward(action);
  update_stats(action, reward);
}

Action AgentGreedy::select() const {
  const double chance = m_bernoulli(m_gen);

  return chance ? Policy_Random() : Policy_Greedy();
}

void AgentGreedy::update_stats(const Action& action, double reward) {
  // auto it = std::find_if(m_actions.begin(), m_actions.end(), [&action](const auto& e_action){
  //   return e_action.action == action;
  // });
  auto it = std::find(m_actions.begin(), m_actions.end(), action);
  ++it->visits;
  it->total += it->rewards.emplace_back(reward);
}

Action AgentGreedy::Policy_Greedy() const {
  assert(not m_actions.empty() && "Agent has empty actions buffer");

  // std::shuffle(m_actions.begin(), m_actions.end(),
  // std::uniform_int_distribution<>(0, m_actions.size()-1));
  return *std::max_element(m_actions.begin(), m_actions.end(), CmpSampleAverage{});
                           // [&](const auto &a, const auto &b) {
                           //   return Q_SampleAverage(a) < Q_SampleAverage(b);
                           // });
}

Action AgentGreedy::Policy_Random() const {
  return m_actions[m_uniform(m_gen)].action;
}

void AgentGreedy::reset(double epsilon) {
  m_time = 0;
  m_actions.clear();
  std::generate_n(std::back_inserter(m_actions), m_bandit.number_of_actions(),
                  [i = 0]() mutable { return ExtAction(i++); });
  m_epsilon = epsilon;
  m_bernoulli = std::bernoulli_distribution(epsilon);
  m_gen.seed(m_rd());
}

ExtAction::ExtAction(size_t idx) : action{idx} {}
bool ExtAction::operator==(const Action a) const { return a == this->action; }
// bool ExtAction::operator<(const ExtAction &other) const {
//   return visits == 0         ? (other.total > 0)
//          : other.visits == 0 ? (total < 0)
//                              : total / visits < total / visits;
// }

CmpSampleAverage::CmpSampleAverage(double prior) : m_prior{prior} {}
double CmpSampleAverage::QFunction(const ExtAction &a) const {
  return a.visits > 0 ? a.total / a.visits : m_prior;
}
bool CmpSampleAverage::operator()(const ExtAction &a,
                                  const ExtAction &b) const {
  return QFunction(a) > QFunction(b);
}
