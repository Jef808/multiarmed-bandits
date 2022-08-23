/**
 * Let ${a_1, \ldots, a_n}$ be the possible actions which
 * can be taken at each time step.
 *
 * We denote by $q(a)$ the expected reward from choosing
 * action $a$,
 * and by $Q_t(a)$ the estimated value on the $t$th time step.
 */
#ifndef AGENT_H_
#define AGENT_H_

#include "multiarmed_bandits.h"
#include "policy.h"

#include <cstddef>
#include <cstdlib>
#include <numeric>
#include <random>

/**
 * Agent that mostly makes simple greedy choices according
 * to the highest average rewards observed in the past,
 * but who for some small probability `epsilon` will make
 * a random choice.
 */
class AgentGreedy {
  using ExtAction = ExtActionT< Action >;

public:
  explicit AgentGreedy(NArmedBandit &bandit, double epsilon=0.0);

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
  void update_stats(const Action& action, double reward);

  /**
   * Resets the agent's data and optional set
   * a new epsilon value.
   */
  void reset(double epsilon=0.0);

  size_t n_actions() const { return m_bandit.number_of_actions(); }

private:
  size_t m_time{0};
  NArmedBandit &m_bandit;
  std::vector<ExtAction> m_actions;

  Policy_Greedy m_greedy{};
  Policy_Random m_random{};
  //  CmpSampleAverage m_Cmp;

  double m_initial_estimate{0.0};
  double m_epsilon{0.0};
  mutable std::bernoulli_distribution m_bernoulli{m_epsilon};
  mutable std::uniform_int_distribution<> m_uniform;
  std::random_device m_rd;
  mutable std::mt19937 m_gen{m_rd()};
};

#endif // AGENT_H_
