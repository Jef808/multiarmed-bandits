#ifndef MULTIARMED_BANDITS_H_
#define MULTIARMED_BANDITS_H_

#include <cstddef>
#include <string>
#include <random>
#include <map>
#include <vector>

#include "policies/extactions.h"

class MultiArmedBandit {
  public:

    MultiArmedBandit() = default;

    /**
     * @Param number_of_actions  The number of available
     * actions to choose from.
     */
    explicit MultiArmedBandit(size_t number_of_actions,
                              uint32_t seed = std::random_device{}());

    void seed(uint32_t seed) { m_gen.seed(seed); }

    /**
     * Generate random values for the expectation
     * of each actions following a normal distribution
     * of mean 0 and variance 1.
     */
    void reset(size_t N);

    /**
     * The (fixed) number of available actions at any time.
     */
    [[nodiscard]] size_t number_of_actions() const { return N; }

    /**
     * Set the number of available actions.
     */
    void set_number_of_actions(size_t nb_action);

    /**
     * Get the reward for choosing a given action.
     *
     * To obtain the reward, perturb the corresponding
     * expected value by random gaussian noise of mean
     * 0 and variance 1.
     */
    [[nodiscard]] double get_reward(const policy::Action& action);

    /**
     * View the real expected reward for the given action.
     */
    [[nodiscard]] double expectation(const policy::Action& action);

    /**
     * Compare the actions' expected values.
     */
    bool operator==(const MultiArmedBandit& other) const;

  private:
    /* Generator for obtaining the needed random values. */
    std::mt19937 m_gen;
    /* Distribution to generate noise in samples. */
    std::normal_distribution<> m_dist{0, 1};
  /* The number of actions. */
    size_t N{0};
  /* The expected values of the actions. */
    std::map<policy::Action, double> m_values;
};

#endif // MULTIARMED_BANDITS_H_
