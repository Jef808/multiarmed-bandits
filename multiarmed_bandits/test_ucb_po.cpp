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

template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v) {
  std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, " "));
  return out;
}

auto get_output_files(std::string_view default_output) {
  using parseopts::vm;

  std::vector<std::string> ret;
  if (vm.count("output-file")) {
    ret = vm["output-file"].as<std::vector<std::string>>();
  } else {
    ret.push_back(default_output.data());
  }
  return ret;
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
  const std::string default_output_file = IN_VIEWER_DATA_DIR "ucb_out.json";

  /////////////////////////////////////////
  // Parse the command line arguments.
  /////////////////////////////////////////
  if (not parseopts::parse(argc, argv)) {
    return EXIT_SUCCESS;
  }
  size_t actions = vm["actions"].as<int>();
  size_t steps = vm["steps"].as<int>();
  size_t stepsize = vm["stepsize"].as<int>();
  size_t episodes = vm["episodes"].as<int>();
  double exploration = vm["exploration"].as<double>();
  std::vector<std::string> output_files = get_output_files(default_output_file);

  /////////////////////////////////////////
  // Declare the statistics to log.
  /////////////////////////////////////////
  stats::Store stats_store;
  // The reward at each step
  auto &rewards = stats_store.add<stats::Type::Series>("rewards");
  // The loss at each step
  auto &losses = stats_store.add<stats::Type::Series>("losses");
  // Each action's visits count at each step
  auto &action_visits =
      stats_store.add<stats::Type::WideSeries>("action_visits");
  // Each action's cumulated average reward at each step
  auto &action_values =
      stats_store.add<stats::Type::WideSeries>("action_values");
  // Each action's value estimation from the policy at each step
  auto &action_pvalues =
      stats_store.add<stats::Type::WideSeries>("action_pvalues");
  // Each action's real expected value
  auto &action_qvalues = stats_store.add<stats::Type::Series>("action_qvalues");

  // Initialize the statistics buffers.
  rewards.resize(steps, 0.0);
  losses.resize(steps, 0.0);
  action_visits.resize(steps, std::vector<double>(actions, 0.0));
  action_values.resize(steps, std::vector<double>(actions, 0.0));
  action_pvalues.resize(steps, std::vector<double>(actions, 0.0));

  /////////////////////////////////////////
  // Generate the model and initialize the agent.
  /////////////////////////////////////////
  // The model
  NArmedBandit bandit{actions};
  // The agent.
  Agent<Policy_UCB> agent(bandit, Policy_UCB{exploration});

  // Store the generated model's action values.
  std::generate_n(
      std::back_inserter(action_qvalues), bandit.number_of_actions(),
      [&bandit, n = 0]() mutable { return bandit.expectation(n++); });

  /////////////////////////////////////////
  // Start the training process.
  /////////////////////////////////////////
  for (size_t n = 0; n < episodes; ++n) {

    // Sample the environment for the designated number of steps.
    for (size_t s = 0; s < steps; ++s) {

      // Variables to accumulate metrics for one step.
      double step_reward = 0.0;
      double step_loss = 0.0;

      // Sample the model 'stepsize' times.
      for (size_t i = 0; i < stepsize; ++i) {
        auto [action, reward] = agent.sample();

        step_reward += reward;
        step_loss += loss(bandit, action);
      }

      // Collect average rewards and losses for each step.
      rewards[s] += step_reward / stepsize;
      losses[s] += step_loss / stepsize;

      // Collect the agent's statistics for each step.
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

    // At the end of each episode, reset the agent's memory.
    agent.reset();
  }

  // Average collected statistics over the number of episodes.
  divide_by(episodes, rewards);
  divide_by(episodes, losses);

  std::for_each(action_visits.begin(), action_visits.end(),
                [episodes](auto &visits) { divide_by(episodes, visits); });
  std::for_each(action_values.begin(), action_values.end(),
                [episodes](auto &values) { divide_by(episodes, values); });
  std::for_each(action_pvalues.begin(), action_pvalues.end(),
                [episodes](auto &pvalues) { divide_by(episodes, pvalues); });

  // Log the collected statistics
  try {
    jsonlog::log("ucb", stats_store, output_files[0]);
    std::cerr << "Logged statistics to " << output_files[0] << "\n";
  } catch (std::exception &e) {
    std::cerr << "Failed while dumping log to " << output_files[0] << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
