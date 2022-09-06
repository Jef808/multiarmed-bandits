#include "agent.h"
#include "jsonlog.h"
#include "multiarmed_bandits.h"
#include "po_helper.h"
#include "policies.h"
#include "statistics.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <fmt/core.h>

template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v) {
  std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, " "));
  return out;
}

inline double loss(NArmedBandit &bandit, const Action &action) {
  double best_val = bandit.expectation(bandit.best_action());
  return std::max(0.0, best_val - bandit.expectation(action));
}

inline void divide_by(double d, std::vector<double> &series) {
  std::transform(series.begin(), series.end(), series.begin(),
                 [d](double v) { return std::divides<>{}(v, d); });
};

////////////////////////////////////////////////////////////
// BEGINNING OF MAIN METHOD
////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
  using parseopts::vm;

  if (not parseopts::parse(argc, argv)) {
    return EXIT_SUCCESS;
  }

  auto output_file = [as_default = IN_VIEWER_DATA_DIR "ucb_out.json"] {
    std::vector<std::string> ret;
    if (vm.count("output-file")) {
      ret = vm["output-file"].as<std::vector<std::string>>();
    } else {
      ret.push_back(as_default);
    }
    return ret;
  }();

  // The variables "actions", "episodes", "steps" and "stepsize" are assigned
  // repeatedly during training process, so we perform their value extraction
  // outside of the loop.
  size_t actions = vm["actions"].as<int>();
  size_t steps = vm["steps"].as<int>();
  size_t stepsize = vm["stepsize"].as<int>();
  size_t episodes = vm["episodes"].as<int>();

  // Construct the model and agent.
  NArmedBandit bandit{actions};
  Agent<Policy_UCB> agent(bandit, Policy_UCB{vm["exploration"].as<double>()});

  stats::Store stats_store;

  // Declare the learning stats to log.
  auto &rewards = stats_store.add<stats::Type::Series>("rewards");
  auto &losses = stats_store.add<stats::Type::Series>("losses");

  // Initialize the buffers
  rewards.resize(steps, 0.0);
  losses.resize(steps, 0.0);

  auto &action_visits =
      stats_store.add<stats::Type::WideSeries>("action_visits");
  auto &action_values =
      stats_store.add<stats::Type::WideSeries>("action_values");
  auto &action_pvalues =
      stats_store.add<stats::Type::WideSeries>("action_pvalues");

  // Initialize the buffers.
  action_visits.resize(steps, std::vector<double>(actions, 0.0));
  action_values.resize(steps, std::vector<double>(actions, 0.0));
  action_pvalues.resize(steps, std::vector<double>(actions, 0.0));

  // Real expected value for each action
  auto &action_qvalues = stats_store.add<stats::Type::Series>("action_qvalues");

  // Store each action's real expected value.
  std::generate_n(
      std::back_inserter(action_qvalues), bandit.number_of_actions(),
      [&bandit, n = 0]() mutable { return bandit.expectation(n++); });

  // Buffer to receive agent samples.
  Action action{};
  double reward{};

  // Start the training process
  for (size_t n = 0; n < episodes; ++n) {

    // Sample the environment for the designated number of steps.
    for (size_t s = 0; s < steps; ++s) {

      double step_reward = 0.0;
      double step_loss = 0.0;

      for (size_t i = 0; i < stepsize; ++i) {
        std::tie(action, reward) = agent.sample();

        step_reward += reward;
        step_loss += loss(bandit, action);
      }

      // We collect rewards and losses for each step.
      rewards[s] += step_reward / stepsize;
      losses[s] += step_loss / stepsize;

      // We collect individual actions statistics for each step.
      {
        // Update action visits statistics
        const std::vector<double> &visits = agent.get_action_visits();
        std::transform(
            visits.begin(), visits.end(), action_visits[s].begin(),
            action_visits[s].begin(),
            [](auto new_val, auto old_val) { return new_val + old_val; });
        // Update action visits statistics
        const std::vector<double> &values = agent.get_action_values();
        std::transform(
            values.begin(), values.end(), action_values[s].begin(),
            action_values[s].begin(),
            [](auto new_val, auto old_val) { return new_val + old_val; });
        // Update action visits statistics
        const std::vector<double> &pvalues = agent.get_action_policy_values();
        std::transform(
            pvalues.begin(), pvalues.end(), action_pvalues[s].begin(),
            action_pvalues[s].begin(),
            [](auto new_val, auto old_val) { return new_val + old_val; });
      }
    }

    // Reset the agent's memory in preparation for the next round.
    agent.reset();
  }

  // Make all statistics average over episodes
  divide_by(episodes, rewards);
  divide_by(episodes, losses);

  std::for_each(action_visits.begin(), action_visits.end(),
                [episodes](auto &visits) { divide_by(episodes, visits); });
  std::for_each(action_values.begin(), action_values.end(),
                [episodes](auto &values) { divide_by(episodes, values); });
  std::for_each(action_pvalues.begin(), action_pvalues.end(),
                [episodes](auto &pvalues) { divide_by(episodes, pvalues); });

  // Log results
  try {
    jsonlog::log("ucb", stats_store, output_file[0]);
  } catch (std::exception &e) {
    std::cerr << "Failed to dump the json output to file " << output_file[0]
              << "\n";
  }

  return EXIT_SUCCESS;
}
