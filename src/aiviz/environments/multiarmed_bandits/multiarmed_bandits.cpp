#include "multiarmed_bandits.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <random>
#include <utility>

#include "aiviz/environments/actions.h"

namespace env {

MultiArmedBandit::MultiArmedBandit(size_t number_of_actions, uint32_t seed)
    : m_gen{seed} {
    reset(number_of_actions);
}

bool MultiArmedBandit::operator==(const MultiArmedBandit& other) const {
    return m_values == other.m_values;
}

void MultiArmedBandit::reset(size_t N) {
    this->N = N;
    m_values.clear();
    std::vector<std::pair<Action, double>> actions_vals{};
    std::generate_n(std::back_inserter(actions_vals), N, [n=0UL]() mutable {
      return std::make_pair( Action{ n++ }, 0.0 );
    });

    m_values.insert(actions_vals.begin(), actions_vals.end());
}

double MultiArmedBandit::get_reward(const Action& action) {
    assert(N > 0);  // NOLINT
    return m_values[action] + m_dist(m_gen);
}

double MultiArmedBandit::expectation(const Action& action) {
    assert(action.id < N);  // NOLINT
    return m_values[action];
}

void MultiArmedBandit::set_number_of_actions(size_t nb_actions) {
    N = nb_actions;
}
}
