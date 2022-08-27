#include "agent.h"
#include "jsonlog.h"
#include "multiarmed_bandits.h"
#include "po_helper.h"
#include "policies.h"

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

////////////////////////////////////////////////////////////
// BEGINNING OF MAIN METHOD
////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
  using parseopts::vm;

  parseopts::parse(argc, argv);

  auto output_file = [as_default = IN_VIEWER_DATA_DIR "ucb_out.json"] {
    std::vector<std::string> ret;
    if (vm.count("output-file")) {
      ret = vm["output-file"].as<std::vector<std::string>>();
    } else {
      ret.push_back(as_default);
    }
    return ret;
  }();

  // The variables "steps" and "stepsize" are assigned repeatedly
  // during training process, so we perform their value extraction
  // outside of the loop.
  size_t steps = vm["steps"].as<int>();
  size_t stepsize = vm["stepsize"].as<int>();

  NArmedBandit bandit{(size_t)vm["actions"].as<int>()};
  Agent<Policy_UCB> agent(bandit, Policy_UCB{vm["exploration"].as<double>()});

  // Cumulative rewards for each step.
  std::vector<double> rewards;
  rewards.resize(steps, 0.0);

  // Cumulative losses for each step.
  std::vector<double> losses;
  losses.resize(steps, 0.0);

  Action action{};
  double reward{};

  int total_visits;
  std::vector<int> action_visits;
  std::vector<double> action_values;
  std::vector<double> action_pvalues;
  std::vector<double> action_qvalues;
  std::generate_n(std::back_inserter(action_qvalues),
                  bandit.number_of_actions(), [&bandit, n = 0]() mutable {
                    return bandit.expectation(Action{static_cast<size_t>(n++)});
                  });

  // Start the training process
  for (size_t n = 0; n < vm["episodes"].as<int>(); ++n) {

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

      // If debugging
      if (vm.count("debug")) {
        action_visits.clear();
        action_values.clear();
        action_pvalues.clear();
        agent.get_action_visits(action_visits);
        agent.get_action_values(action_values);
        agent.get_action_policy_values(action_pvalues);

        fmt::print("Step {}\n", s);
        for (size_t a = 0; a < bandit.number_of_actions(); ++a) {
          fmt::print("Action {0}: Visits = {1}, Value = {2}, PolicyValue = "
                     "{3}, TrueValue = {4}\n",
                     a, action_visits[a], action_values[a], action_pvalues[a],
                     action_qvalues[a]);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }

    // Reset the agent's memory in preparation for the next round.
    agent.reset();
  }

  // Make each entry equal to the average over the episodes
  for (size_t s = 0; s < steps; ++s) {
    rewards[s] /= vm["episodes"].as<int>();
    losses[s] /= vm["episodes"].as<int>();
  }

  // Log results
  try {
    jsonlog::log("ucb", std::move(rewards), std::move(losses), output_file[0]);
  } catch (std::exception &e) {
    std::cerr << "Failed to dump the json output to file " << output_file[0]
              << "\n";
  }

  return EXIT_SUCCESS;
}
