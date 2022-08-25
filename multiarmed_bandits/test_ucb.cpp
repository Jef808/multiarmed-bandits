#include "agent.h"
#include "cmdline_helper.h"
#include "multiarmed_bandits.h"
#include "policies.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

const Options defaults{
    .n_actions = 10,
    .exploration_constant = 0.4,
    .stepsize = 1,
    .n_steps = 500,
    .n_episodes = 500,
    .output = ROOTDIR + "app/public/data/ucb.json",
    .debug = false,
    .help = false,
};

bool json_log(std::string name, std::vector<double> &&rewards,
              std::vector<double> &&losses, const Options &options) {
  using namespace nlohmann;
  json j;

  std::vector<double> _rewards = rewards;
  std::vector<double> _losses = losses;

  j["policy"] = name;
  j["n_actions"] = options.n_actions;
  j["exploration_constant"] = options.exploration_constant;
  j["n_steps"] = options.n_steps;
  j["stepsize"] = options.stepsize;
  j["n_episodes"] = options.n_episodes;
  j["rewards"] = json::array({});
  j["losses"] = json::array({});
  for (auto r : _rewards) {
    j["rewards"].push_back(r);
  }
  for (auto l : _losses) {
    j["losses"].push_back(l);
  }

  std::string_view fn = options.output;

  std::ofstream ofs{ fn.data() };
  if (not ofs) {
    std::cerr << "WARNING: Unable to open log file: " << fn << std::endl;
    return false;
  }

  try {
    ofs << j.dump() << std::endl;
    std::cout << "Wrote output to log file " << fn << std::endl;
    ;
    return true;
  } catch (std::exception &e) {
    std::cerr << "Failed to dump json object to string: " << e.what()
              << std::endl;
    return false;
  }
}

inline double loss(NArmedBandit &bandit, const Action &action) {
  double best_val = bandit.expectation(bandit.best_action());
  return std::max(0.0, best_val - bandit.expectation(action));
}

////////////////////////////////////////////////////////////
// BEGINNING OF MAIN METHOD
////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
  Options opts = defaults;

  parse_opts(argc, argv, opts);

  if (opts.help) {
    print_usage(argv[0]);
    return EXIT_SUCCESS;
  }

  NArmedBandit bandit{opts.n_actions};
  Agent<Policy_UCB> agent(bandit, Policy_UCB{opts.exploration_constant});

  // Cumulative rewards for each step.
  std::vector<double> rewards;
  rewards.resize(opts.n_steps, 0.0);

  // Cumulative losses for each step.
  std::vector<double> losses;
  losses.resize(opts.n_steps, 0.0);

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
  for (size_t n = 0; n < opts.n_episodes; ++n) {

    // Sample the environment for the designated number of steps.
    for (size_t s = 0; s < opts.n_steps; ++s) {

      double step_reward = 0.0;
      double step_loss = 0.0;

      for (size_t i = 0; i < opts.stepsize; ++i) {
        std::tie(action, reward) = agent.sample();

        step_reward += reward;
        step_loss += loss(bandit, action);
      }

      // We collect rewards and losses for each step.
      rewards[s] += step_reward / opts.stepsize;
      losses[s] += step_loss / opts.stepsize;

      if (opts.debug) {
        action_visits.clear();
        action_values.clear();
        action_pvalues.clear();
        agent.get_action_visits(action_visits);
        agent.get_action_values(action_values);
        agent.get_action_policy_values(action_pvalues);

        int a = 0;
        fmt::print("Step {0}\n", s);
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
  for (size_t s = 0; s < opts.n_steps; ++s) {
    rewards[s] /= opts.n_episodes;
    losses[s] /= opts.n_episodes;
  }

  // Log results
  const bool log_okay = json_log("ucb", std::move(rewards), std::move(losses), opts);

  return log_okay ? EXIT_SUCCESS : EXIT_FAILURE;
}
