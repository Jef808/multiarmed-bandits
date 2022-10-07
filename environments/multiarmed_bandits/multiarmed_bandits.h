#ifndef MULTIARMED_BANDITS_H_
#define MULTIARMED_BANDITS_H_

#include <cstddef>
#include <random>
#include <vector>

struct Action {
  size_t idx;
  bool operator==(const Action &other) const { return idx == other.idx; }
};

class NArmedBandit {
public:
  struct Action {
    size_t idx;
    bool operator==(const Action &other) const { return idx == other.idx; }
  };

  /**
   * @Param number_of_actions  The number of available
   * actions to choose from.
   */
  explicit NArmedBandit(size_t number_of_actions);

  /**
   * Generate random values for the expectation
   * of each actions following a normal distribution
   * of mean 0 and variance 1.
   */
  void reset();

  /**
   * The (fixed) number of available actions at any time.
   */
  size_t number_of_actions() const { return N; }

  /**
   * Get the reward for choosing a given action.
   *
   * To obtain the reward, perturb the corresponding
   * expected value by random gaussian noise of mean
   * 0 and variance 1.
   */
  double get_reward(const Action &action);

  /**
   * View the real expected reward for the given action.
   */
  double expectation(const Action &action) const;

  /**
   * View the real expected reward for the given action.
   */
  double expectation(size_t action) const;

  /**
   * Get the action with highest expectation.
   */
  Action best_action() const;

  /**
   * Compare the actions' expected values.
   */
  bool operator==(const NArmedBandit &other) const;

private:
  /* The number of actions. */
  size_t N;
  /* The expected values of the actions. */
  std::vector<double> m_values;
  /* Used to obtain random seeds when resetting. */
  std::random_device m_rd;
  /* Generator for obtaining the needed random values. */
  std::mt19937 m_gen{m_rd()};
  /* The normal distribution which shapes the random values. */
  std::normal_distribution<> m_dist;
  /* The action with the highest expectation. */
  Action m_best_action;
};

#endif // MULTIARMED_BANDITS_H_
