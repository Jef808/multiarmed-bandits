#ifndef AGENT_H_
#define AGENT_H_

/**
 * Let ${a_1, \ldots, a_n}$ be the possible actions which
 * can be taken at each time step.
 *
 * We denote by $q(a)$ the expected reward from choosing
 * action $a$,
 * and by $Q_t(a)$ the estimated value on the $t$th time step.
 */
#include "multiarmed_bandits.h"

#include <cstddef>
#include <cstdlib>
#include <numeric>
#include <random>

constexpr auto default_number_of_actions = 10;

/**
 * Structure to accumulate data along the search.
 */
struct ExtAction;

/**
 * Structure to order ExtAction instances by
 * decreasing values of their sampled averages.
 */
struct CmpSampleAverage;

/**
 * Agent that mostly makes simple greedy choices according
 * to the highest average rewards observed in the past,
 * but who for some small probability `epsilon` will make
 * a random choice.
 */
class AgentGreedy {
public:
  explicit AgentGreedy(NArmedBandit &bandit, double epsilon=0.0);

  void sample(std::vector<ExtAction>& buf, size_t n_steps=1);

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

  /**
   * Estimate the value at time $t$ by averaging the rewards
   * received up to then.
   */
  double Q_SampleAverage(const ExtAction &a) const;

  /**
   * Select the action with the largest estimated
   * expected reward.
   */
  Action Policy_Greedy();

  /**
   * Select an action randomly.
   */
  Action Policy_Random() const;

  /**
   * Access the agent's cumulated data
   */
  void dump_episode_data(std::vector<ExtAction>& buf);

  size_t n_actions() const { return m_bandit.number_of_actions(); }

private:
  size_t m_time{0};
  NArmedBandit &m_bandit;
  std::vector<ExtAction> m_actions;

  double m_initial_estimate{0.0};
  double m_epsilon{0.0};
  mutable std::bernoulli_distribution m_bernoulli{m_epsilon};
  mutable std::uniform_int_distribution<> m_uniform;
  std::random_device m_rd;
  mutable std::mt19937 m_gen{m_rd()};
};

struct ExtAction {
  explicit ExtAction(size_t idx);

  Action action;
  int visits;
  double total;

  operator Action() const { return action; }
  bool operator==(const Action a) const;
};

struct CmpSampleAverage {
  /**
   * @Param prior  The initial guess for the expected value of
   * an action which has not been visited yet.
   */
  explicit CmpSampleAverage(double prior=0.0);

  /* Compute the average reward observed to date. */
  double QFunction(const ExtAction& a) const;
  /* Sort in decreasing order. */
  bool operator()(const ExtAction& a, const ExtAction& b) const;

  /* Initial QValue. */
  double m_prior;
};

#endif // AGENT_H_
