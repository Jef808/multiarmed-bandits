#include "request_handler.h"
#include "environments/multiarmed_bandits/multiarmed_bandits.h"
#include "policies/agent.h"
#include "policies/epsilon_greedy.h"
#include "policies/ucb.h"

#include <algorithm>
#include <nlohmann/json.hpp>
#include <numeric>
#include <variant>

using json = nlohmann::json;

namespace Request {

namespace {

template <typename E> std::underlying_type_t<E> toInt(E e) {
    return static_cast<std::underlying_type_t<E>>(e);
}

auto GetModelVariant(const json& jModel) {
    using namespace serialized::model;
    switch (jModel["name"].get<Name>()) {
    case MultiarmedBandit: return Variant{jModel["parameters"].get<Mab>()};
    default: assert(false); // NOLINT(*array-to-pointer-decay)
    }
}

auto GetPolicyVariant(const json& jPolicy) {
    using namespace serialized::policy;
    switch (jPolicy["name"].get<Name>()) {
    case EpsilonGreedy:
        return Variant{jPolicy["parameters"].get<struct EpsilonGreedy>()};
    case Ucb: return Variant{jPolicy["parameters"].get<struct Ucb>()};
    default: assert(false); // NOLINT(*array-to-pointer-decay)
    }
}

template <typename Ag, typename OutputIter>
OutputIter run(Ag agent, const size_t number_of_results, OutputIter results) {
    std::generate_n(results, number_of_results, [&agent]() {
        const auto& [_action, value] = agent.sample();
        return serialized::Sample{_action.idx, value};
    });
    return results;
}

template <typename OutputIter> struct Visitor {

    Visitor(size_t nb_results, OutputIter results_out)
        : number_of_results{nb_results}
        , results_output{results_out} { }

    void operator()(serialized::model::Mab m,
                    struct serialized::policy::EpsilonGreedy p) {
        run(Agent{MultiArmedBandit(m.nbOfArms),
                  policy::EpsilonGreedy(p.epsilon)},
            number_of_results, results_output);
    }

    void operator()(serialized::model::Mab m,
                    struct serialized::policy::Ucb p) {
        Agent agent(MultiArmedBandit(m.nbOfArms), policy::Ucb(p.exploration));
        run(Agent{MultiArmedBandit(m.nbOfArms), policy::Ucb(p.exploration)},
            number_of_results, results_output);
    }

    size_t number_of_results;
    OutputIter results_output;
};
} // namespace

/**
 * Run the request and collect resulting data in our results_buffer vector.
 */
serialized::Result& Handler::process(const char* buf, size_t sz) const {
    std::string_view sv(buf, sz);
    json jrequest = json::parse(sv);

    samples_buffer.clear();
    results_buffer.series_list.clear();

    // Match the result id to the request id.
    auto id = jrequest["id"].get<std::string>();
    results_buffer.id = id;

    auto options = jrequest["options"].get<serialized::Options>();
    const size_t nb_results = options.nbOfSteps;
    const size_t avg_window_size = options.averagingWindowSize;

    // Collect the samples
    std::visit(Visitor{nb_results, std::back_inserter(samples_buffer)},
               GetModelVariant(jrequest["model"]),
               GetPolicyVariant(jrequest["policy"]));

    // Construct each series
    for (const std::string& result_name : {"value"}) {

        serialized::TimeSeries& result_series =
            results_buffer.series_list.emplace_back();

        result_series.name = result_name;
        std::vector<serialized::TimePoint>& result_values =
            result_series.values;

        const auto window_width = [&nb_results,
                                   avg_win_sz = avg_window_size](int step) {
            return step == 0 ? nb_results % avg_win_sz : avg_win_sz;
        };

        const int nb_of_windows =
            avg_window_size == 0 ? 1
                                 : (int)(std::round((double)nb_results
                                                    / (double)avg_window_size))
                                       + 1;
        std::generate_n(
            std::back_inserter(result_values), nb_of_windows,
            [&, step = 0]() mutable {
                serialized::TimePoint window;
                const size_t width = window_width((int)step);

                auto& [window_step, window_value, window_count] =
                    result_values.emplace_back();

                window_step = step + window_width(width);

                std::tie(window_value, window_count) = std::accumulate(
                    samples_buffer.begin() + (int)step,
                    samples_buffer.begin() + (int)window_step,
                    std::make_pair(0.0, std::map<std::string, int>{}),
                    [](auto&& s, const serialized::Sample& c) {
                        s.first += c.value;
                        auto [it, count] =
                            s.second.try_emplace(c.action.to_string(), 0);
                        it->second = count + 1;
                        return std::move(s);
                    });

                step = window_step;
                if (width > 0) {
                    window_value /= (int)width;
                }

                return window;
            });
    }

    return results_buffer;
}

const std::string& Request::Handler::operator()(const std::string& req) const {
    json j = process(req.data(), req.size());
    return (str_buffer = j.dump());
}
} // namespace Request
