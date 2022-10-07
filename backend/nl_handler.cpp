#include "../environments/multiarmed_bandits/multiarmed_bandits.h"
#include "../policies/agent.h"
#include "../policies/policy_greedy.h"
#include "../policies/policy_ucb.h"
#include "data_models.hpp"
#include "nl_handler.h"

#include <algorithm>
#include <exception>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>

using json = nlohmann::json;
using namespace nlohmann::literals;

template <class... Ts> struct overload : Ts... { using Ts::operator()...; };

using model_var_t = std::variant<data::model::mab_t>;
using policy_var_t =
    std::variant<data::policy::epsilon_greedy_t, data::policy::ucb_t>;

template <typename M, typename P>
data::result_t<typename M::Action> run(Agent<M, P>& agent,
                                       const data::parameters_t& params) {

    data::result_t<typename M::Action> result;
    result.reserve(params.numberOfSteps);

    std::generate_n(std::back_inserter(result), params.numberOfSteps, [&agent] {
        auto [action, value] = agent.sample();
        agent.update_stats(action, value);
        return wrap_sample(action, value);
    });

    return result;
}

void handle_request(const json& req, json& rep) {

    model_var_t model_var;
    policy_var_t policy_var;

    switch (req["model"]["name"].get<data::model::Name>()) {
    case data::model::N_Mab:
        model_var = req["model"]["parameters"].get<data::model::mab_t>();
        break;
    default: throw std::runtime_error("Invalid model name");
    }

    switch (req["policy"]["name"].get<data::policy::Name>()) {
    case data::policy::N_EpsilonGreedy:
        policy_var =
            req["policy"]["parameters"].get<data::policy::epsilon_greedy_t>();
        break;
    case data::policy::N_Ucb:
        policy_var = req["policy"]["parameters"].get<data::policy::ucb_t>();
        break;
    default: throw std::runtime_error("Invalid policy name");
    }

    auto parameters = req["parameters"].get<data::parameters_t>();

    std::visit(
        overload{[&parameters, &rep](const data::model::mab_t& m,
                                     const data::policy::epsilon_greedy_t& p) {
                     auto model = NArmedBandit{
                         static_cast<size_t>(parameters.numberOfSteps)};
                     auto policy = Policy_Greedy{p.epsilon};
                     Agent agent(model, policy);

                     rep = run(agent, parameters);
                 },
                 [&parameters, &rep](const data::model::mab_t& m,
                                     const data::policy::ucb_t& p) {
                     auto model = NArmedBandit{
                         static_cast<size_t>(parameters.numberOfSteps)};
                     auto policy = Policy_UCB{p.exploration};
                     Agent agent(model, policy);

                     rep = run(agent, parameters);
                 }},
        model_var, policy_var);
}
