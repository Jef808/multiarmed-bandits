#ifndef AGENT_H_
#define AGENT_H_

#include <algorithm>
#include <cassert>
#include <random>

#include "policies/extactions.h"

template <typename Model, typename Policy> class Agent {
  public:
    Agent(Model&& model, Policy&& policy)
        : model_{std::move(model)}
        , policy_{std::move(policy)} {
        reset();
    }

    /**
     * Sample the model according to the policy's current state.
     */
    std::pair<typename Model::Action, double> sample();

    /**
     * View the agent's current data on each actions.
     */
    const std::vector<ExtAction<typename Model::Action>>&
        current_actions_info() const {
        return actions_data;
    }

    /**
     * To access the agent's policy (for logging/debugging).
     */
    const Policy& policy() { return policy_; }

    void seed_model() { model_.seed(rd()); }

    void seed_policy() { policy_.seed(rd()); }

    /**
     * Resets the agent's data with small gaussian noise as values.
     *
     * TODO: Allow generic models (currently asssumes model is MultiArmedBandit)
     */
    void reset();

  private:
    std::random_device rd{};
    std::mt19937 gen{rd()};

    // The model the agent is operating on.
    mutable Model model_;

    // The policy the agent is following.
    // Specified mutable because it uses RNG which has state
    mutable Policy policy_;

    // Storage for the agent's accumulated knowledge.
    std::vector<ExtAction<typename Model::Action>> actions_data;

    /**
     * Update the agent's statistics after a
     * newly sampled action.
     */
    void update_stats(const typename Model::Action& action, double reward);
};

template <typename Model, typename Policy>
inline std::pair<typename Model::Action, double>
    Agent<Model, Policy>::sample() {
    typename Model::Action action = policy_(actions_data);

    double reward = model_.get_reward(action);
    update_stats(action, reward);

    return std::make_pair(action, reward);
}

template <typename Model, typename Policy>
inline void
    Agent<Model, Policy>::update_stats(const typename Model::Action& action,
                                       double reward) {
    auto it = std::find(actions_data.begin(), actions_data.end(), action);
    assert(it != actions_data.end());

    it->visits += 1;
    it->total += reward;
}

template <typename Model, typename Policy>
inline void Agent<Model, Policy>::reset() {
    actions_data.clear();
    gen.seed(rd());
    model_.reset();

    std::generate_n(std::back_inserter(actions_data),
                    model_.number_of_actions(),
                    [dist = std::normal_distribution<>(0.0, 0.1), n = 0,
                     &g = gen]() mutable {
                        ExtAction<typename Model::Action> ret(n++);
                        ret.visits = 0;
                        ret.total = dist(g);
                        return ret;
                    });
}

#endif // AGENT_H_
