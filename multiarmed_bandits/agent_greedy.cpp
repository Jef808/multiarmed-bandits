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
    : m_bandit{bandit}, m_actions{}, m_epsilon{epsilon},
      m_uniform(0, bandit.number_of_actions() - 1) {
  for (size_t i = 0; i < n_actions(); ++i) {
    auto &ea = m_actions.emplace_back(i);
  }
}

void AgentGreedy::sample(std::vector<ExtAction> &buf, size_t n_steps) {
  assert(buf.size() == n_actions() && "Data buffer wrongly initialized");

  m_gen.seed(m_rd());

  for (size_t i = 0; i < n_steps; ++i) {
    Action action = select();
    double reward = m_bandit.get_reward(action);
    update_stats(action, reward);
    ++buf[action.idx].visits;
    buf[action.idx].total += reward;
  }

  //dump_episode_data(buf);
}

Action AgentGreedy::select() {
  const double chance = m_bernoulli(m_gen);
  ++m_time;
  return chance ? Policy_Random() : Policy_Greedy();
}

void AgentGreedy::update_stats(const Action &action, double reward) {
  // auto it = std::find(m_actions.begin(), m_actions.end(), action);
  ExtAction &ea = m_actions[action.idx];
  assert(ea.action == action && "update_stats(): Actions are out of order");
  ea.visits += 1;
  ea.total += reward;
}

void AgentGreedy::dump_episode_data(std::vector<ExtAction> &buf) {
  for (size_t i = 0; i < n_actions(); ++i) {
    ExtAction &ea = buf[i];
    ExtAction &this_ea = m_actions[i];

    assert(ea.action == this_ea.action && "Action indices don't match");

    ea = this_ea;
  }
}

Action AgentGreedy::Policy_Greedy() {
  static std::vector<Action> candidates;
  CmpSampleAverage cmp{m_initial_estimate};

  assert(not m_actions.empty() && "Agent has empty actions buffer");

  candidates.clear();
  double max_val =
      cmp.QFunction(*std::max_element(m_actions.begin(), m_actions.end(), cmp));

  std::copy_if(m_actions.begin(), m_actions.end(),
               std::back_inserter(candidates),
               [&cmp, v = max_val](const auto &ea) {
                 return cmp.QFunction(ea) > v - 0.00001;
               });

  std::uniform_int_distribution<> dist(0, candidates.size() - 1);
  int idx = dist(m_gen);

  return m_actions[idx];
}

Action AgentGreedy::Policy_Random() const {
  return m_actions[m_uniform(m_gen)].action;
}

// void AgentGreedy::reset(double epsilon) {
//   m_time = 0;
//   m_epsilon = epsilon;
//   m_bernoulli = std::bernoulli_distribution(epsilon);
//   m_actions.clear();
//   m_gen.seed(m_rd());
// }

ExtAction::ExtAction(size_t idx) : action{idx}, visits{0}, total{0} {}

bool ExtAction::operator==(const Action a) const { return a == this->action; }

CmpSampleAverage::CmpSampleAverage(double prior) : m_prior{prior} {}
double CmpSampleAverage::QFunction(const ExtAction &a) const {
  return a.visits > 0 ? a.total / a.visits : m_prior;
}
bool CmpSampleAverage::operator()(const ExtAction &a,
                                  const ExtAction &b) const {
  return QFunction(a) < QFunction(b);
}
