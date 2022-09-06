#include "multiarmed_bandits.h"

#include <algorithm>
#include <random>

NArmedBandit::NArmedBandit(size_t number_of_actions)
    : N{number_of_actions}, m_values{} {
  m_dist = std::normal_distribution<>(0.0, 1.0);
  reset();
}

void NArmedBandit::reset() {
  m_gen.seed(m_rd());
  std::generate_n(std::back_inserter(m_values), N,
                  [&]() { return m_dist(m_gen); });

  m_best_action.idx = std::distance(
      m_values.begin(), std::max_element(m_values.begin(), m_values.end()));
}

double NArmedBandit::get_reward(const Action &action) {
  return m_values[action.idx] + m_dist(m_gen);
}

double NArmedBandit::expectation(const Action &action) const {
  return m_values[action.idx];
}

double NArmedBandit::expectation(size_t action) const {
  return m_values[action];
}

Action NArmedBandit::best_action() const { return m_best_action; }
