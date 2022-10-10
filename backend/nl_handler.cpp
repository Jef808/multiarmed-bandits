#include "nl_handler.h"
#include "environments/multiarmed_bandits/multiarmed_bandits.h"
#include "policies/agent.h"
#include "policies/epsilon_greedy.h"
#include "policies/ucb.h"
#include "serialized.h"

#include <algorithm>
#include <nlohmann/json.hpp>
#include <variant>

using json = nlohmann::json;

namespace {

auto GetModelVariant(const json& jModel) {
    using namespace serialized::model;
    switch (jModel["name"].get<Name>()) {
    case (kMab): return Variant{jModel["parameters"].get<Mab>()};
    default: assert(false); // NOLINT(*array-to-pointer-decay)
    }
}

auto GetPolicyVariant(const json& jPolicy) {
    using namespace serialized::policy;
    switch (jPolicy["name"].get<Name>()) {
    case (kEpsilonGreedy):
        return Variant{jPolicy["parameters"].get<EpsilonGreedy>()};
    case (kUcb): return Variant{jPolicy["parameters"].get<Ucb>()};
    default: assert(false); // NOLINT(*array-to-pointer-decay)
    }
}

template <typename Ag, typename OutputIter>
void run(Ag agent, const size_t number_of_results, OutputIter results) {
    std::generate_n(results, number_of_results, [&agent]() {
        const auto& [action, value] = agent.sample();
        return serialized::sample(action.to_string(), value);
    });
}

template <typename OutputIterator> struct Visitor {

    Visitor(size_t nb_results, OutputIterator results_out)
        : number_of_results{nb_results}
        , results_output{results_out} { }

    void operator()(serialized::model::Mab m,
                    serialized::policy::EpsilonGreedy p) {
        run(Agent{MultiArmedBandit(m.numberOfArms),
                  policy::EpsilonGreedy(p.epsilon)},
            number_of_results, results_output);
    }

    void operator()(serialized::model::Mab m, serialized::policy::Ucb p) {
        Agent agent(MultiArmedBandit(m.numberOfArms),
                    policy::Ucb(p.exploration));
        run(Agent{MultiArmedBandit(m.numberOfArms), policy::Ucb(p.exploration)},
            number_of_results, results_output);
    }

    size_t number_of_results;
    OutputIterator results_output;
};

json process(json request, std::vector<serialized::sample>& buffer) {
    auto params = request["parameters"].get<serialized::parameters>();
    size_t nb_results = params.numberOfSteps;

    buffer.resize(nb_results, {std::string(), 0});

    std::visit(Visitor{nb_results, buffer.begin()},
               GetModelVariant(request["model"]),
               GetPolicyVariant(request["policy"]));

    return {buffer};
}

} // namespace

std::string request_handler_t::operator()(const std::string& request) {
    return process(json::parse(request), results_buffer).dump();
}
