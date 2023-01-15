#ifndef AGENT_H_
#define AGENT_H_

#include "policy.h"

#include <utility>
#include <vector>

template<template <class> typename PolicyT, typename State>
class Agent {
    using Action = typename State::Action;
    using Policy = PolicyT< Action >;
    using policy::ExtAction = policy::ExtActionT< Action >;

public:
    Agent(State& state, Policy&& policy);

    std::pair<Action, double> sample();

    void update_stats(const Action& action, double reward);

    void reset();

private:
    State& m_state;
    Policy m_policy;
    std::vector< policy::ExtAction > m_actions;
};

template<template <class> typename PolicyT, typename State>
Agent<PolicyT, State>::Agent(State& state, Policy&& policy)
    : m_state{state}, m_policy{policy}
{}

template<template <class> typename PolicyT, typename State>
std::pair< typename State::Action, double >
Agent<PolicyT, State>::sample() {
    m_actions = m_state.valid_actions();
    policy::ExtAction ea = m_policy(m_actions);
}




#endif // AGENT_H_
