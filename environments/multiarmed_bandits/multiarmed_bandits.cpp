#include "multiarmed_bandits.h"

#include <algorithm>
#include <cassert>
#include <random>

MultiArmedBandit::MultiArmedBandit(size_t number_of_actions, uint32_t seed)
    : N{number_of_actions}
    , m_gen{seed} {
    reset();
}

bool MultiArmedBandit::operator==(const MultiArmedBandit& other) const {
    return m_values == other.m_values;
}

void MultiArmedBandit::reset() {
    m_values.clear();
    std::generate_n(std::back_inserter(m_values), N,
                    [&]() { return m_dist(m_gen); });
}

double MultiArmedBandit::get_reward(const Action& action) {
    assert(N > 0);
    return m_values[action.idx] + m_dist(m_gen);
}

double MultiArmedBandit::expectation(const Action& action) const {
    assert(action.idx >= 0 && action.idx < N);
    return m_values[action.idx];
}

// double NArmedBandit::expectation(size_t action) const {
//     return m_values[action];
// }
