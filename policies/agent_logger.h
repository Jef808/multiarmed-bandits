/**
 * These helper functions' purpose is to get access to an agent's
 * running evaluations and memory for each actions while it is
 * running (for benchmarking or debugging purposes).
 */
#ifndef AGENT_LOGGER_H_
#define AGENT_LOGGER_H_

#include <algorithm>
#include <vector>

namespace agentLogging {

/**
 * View the agent's current evaluation of each actions.
 */
template < typename AgentT >
void get_action_values(const AgentT& agent, std::vector<double>& out) {
    const auto& agent_data = agent.current_actions_info();
    std::transform(agent_data.begin(), agent_data.end(),
                   std::back_inserter(out), [](const auto& ea) {
                       return ea.visits == 0 ? ea.total : ea.total / ea.visits;
                   });
}

/**
 * View the current number of times the agent has visited each action.
 */
template <typename AgentT>
void get_action_visits(const AgentT& agent, std::vector<double>& out) {
    const auto& agent_data = agent.current_actions_info();
    std::transform(agent_data.begin(), agent_data.end(),
                   std::back_inserter(out),
                   [](const auto& ea) { return ea.visits; });
}

/**
 * View evaluation of each actions according to `AgentT`'s policy and its
 * current knowledge of each actions.
 */
template <typename AgentT>
void get_policy_evaluations(const AgentT& agent, std::vector<double>& out) {
    const auto& policy = agent.policy();
    policy.get_action_values(agent.current_actions_info, out);
}

} // namespace agentLogging

#endif // AGENT_LOGGER_H_
