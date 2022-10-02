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
#include <vector>

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

  // Initialize the statistics buffers.
  rewards.resize(steps, 0.0);
  losses.resize(steps, 0.0);

  /////////////////////////////////////////
  // Generate the model and initialize the agent.
  /////////////////////////////////////////
  // The model
  NArmedBandit bandit{actions};
  // The agent.
  Agent<Policy_UCB> agent(bandit, Policy_UCB{exploration});

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
        const auto &[action, reward] = agent.sample();

        step_reward += reward;
        step_loss += loss(bandit, action);
      }

      // Collect average rewards and losses for each step.
      rewards[s] += step_reward / stepsize;
      losses[s] += step_loss / stepsize;
    }

    // At the end of each episode, reset the agent's memory
    // and regenerate new random values for the model's action
    // values.
    agent.reset();
    bandit.reset();
  }

  // Average collected statistics over the number of episodes.
  divide_by(episodes, rewards);
  divide_by(episodes, losses);

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
