#ifndef AGENT_H_
#define AGENT_H_

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <tuple>

#include "extactions.h"

namespace policy {


template <typename Model, typename Policy> class Agent {
  public:
    Agent(const Model& model, const Policy& policy)
        : model_{model}, policy_{policy} {
        reset();
    }

    /**
     * Sample the model according to the policy's current state.
     */
    std::pair<Action, double> sample();

    /**
     * Sample the model `nb_samples` of times, each time choosing the
     * action according to the updated policy.
     */
    template<typename OutputIter>
    void run(size_t nb_samples, OutputIter out);

    void set_model(Model model) {
        model_ = model;
        reset();
    }
    void set_policy(Policy policy) {
        policy_ = policy;
        reset();
    }

    /**
     * To access the agent's policy (for logging/debugging).
     */
    const Policy& policy() { return policy_; }

    /**
     * View the agent's accumulated data for each actions.
     */
    const std::vector<ExtAction>&
        current_actions_info() const {
        return actions_data;
    }

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
    std::vector<ExtAction> actions_data;

    /**
     * Update the agent's statistics after a
     * newly sampled action.
     */
    void update_stats(const Action& action, double reward);

    void seed_model() { model_.seed(rd()); }

    void seed_policy() { policy_.seed(rd()); }
};

template <typename Model, typename Policy>
inline std::pair<Action, double>
    Agent<Model, Policy>::sample() {

    const Action action = policy_(actions_data);
    const double reward = model_.get_reward(action);

    update_stats(action, reward);
    return std::make_pair(action, reward);
}

template <typename Model, typename Policy>
inline void
    Agent<Model, Policy>::update_stats(const Action& action, double reward) {
    auto it = std::find(actions_data.begin(), actions_data.end(), action);
    assert(it != actions_data.end());

    it->visits += 1;
    it->total += reward;
}

template<typename Model, typename Policy>
template<typename OutputIter>
inline void Agent<Model, Policy>::run(size_t nb_samples, OutputIter out) {
  std::generate_n(out, nb_samples, [&]{
        out = sample();
    });
}

template <typename Model, typename Policy>
inline void Agent<Model, Policy>::reset() {
    actions_data.clear();
    gen.seed(rd());
    model_.reset(model_.number_of_actions());

    // Initialize the actions accumulators with gaussian noise.
    std::generate_n(std::back_inserter(actions_data),
                    model_.number_of_actions(),
                    [dist = std::normal_distribution<>(0.0, 0.1), n = 0UL,
                     &g = gen]() mutable {
                        ExtAction ret{ Action{n++} };
                        ret.visits = 0;
                        ret.total = dist(g);
                        return ret;
                    });

}

} // policy

#endif // AGENT_H_
