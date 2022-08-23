#include "agent_greedy.h"
#include "multiarmed_bandits.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

constexpr double default_epsilon = 0.1;
constexpr size_t default_n_actions = 10;
constexpr size_t default_n_steps = 1000;
constexpr size_t default_n_episodes = 2000;

const std::string log_dir = "/home/jfa/projects/ai/logs/multiarmed_bandits/";

using ExtAction = ExtActionT<Action>;

inline double loss(const NArmedBandit &bandit, const Action &chosen_action) {
  return bandit.expectation(bandit.best_action()) -
         bandit.expectation(chosen_action);
}

bool parse_opts(int argc, char *argv[], size_t &n_actions, double &epsilon,
                size_t &n_steps, size_t &n_episodes) {

  std::vector<std::string_view> args;
  for (auto i = 0; i < argc; ++i) {
    args.push_back(argv[i]);
  }

  auto it = std::find_if(args.begin(), args.end(), [](auto arg){
    return arg[0] == '-';
  });
  while (it != args.end()) {
    char c = (*it)[1];
    try {
      switch (c) {
      case 'a':
        n_actions = std::stoi((it + 1)->data());
        fmt::print("Set number of actions to {0}\n", n_actions);
        break; // number of actions
      case 'e':
        epsilon = std::stod((it + 1)->data());
        fmt::print("Set epsilon to {0}\n", epsilon);
        break; // epsilon
      case 's':
        n_steps = std::stoi((it + 1)->data());
        fmt::print("Set number of steps to {0}\n", n_steps);
        break; // number of steps
      case 'n':
        n_episodes = std::stoi((it + 1)->data());
        fmt::print("Set number of episodes to {0}\n", n_episodes);
        break; // number of episodes
      default:
        fmt::print("Unrecognized command line argument.");
        break;
      }
    } catch (std::exception &e) {
      fmt::print("Invalid input parameter: {0}\n", e.what());
      return false;
    }
    it = std::find_if(it + 1, args.end(), [](auto arg){
      return arg[0] == '-';
    });
  }
  return true;
}

std::string make_log_fn(size_t n_actions, double epsilon, size_t n_steps, size_t n_episodes) {
  std::stringstream ss;
  int epsilon_pc = 100 * epsilon;
  ss << log_dir << "epsilon_greedy_"
     << n_actions << '_'
     << epsilon_pc << '_'
     << n_steps << '_'
     << n_episodes << ".json";
  return ss.str();
}

void json_log(std::string_view log_fn, nlohmann::json &&j) {
  std::ofstream ofs{log_fn.data()};
  if (not ofs) {
    fmt::print("WARNING: Unable to open log file: {0}\n", log_fn);
  }

  ofs << j.dump(4) << std::endl;
  fmt::print("Wrote output to log file {0}\n", log_fn);
}

int main(int argc, char *argv[]) {
  using namespace nlohmann;

  size_t n_actions = default_n_actions;
  double epsilon = default_epsilon;
  size_t n_steps = default_n_steps;
  size_t n_episodes = default_n_episodes;

  parse_opts(argc, argv, n_actions, epsilon, n_steps, n_episodes);

  NArmedBandit bandit{n_actions};
  AgentGreedy agent{bandit, epsilon};

  // Get the maximal reward expectation to compute a loss function.
  double value_best_action = bandit.expectation(bandit.best_action());

  // Cumulative rewards for each step.
  std::vector<double> rewards;
  rewards.resize(n_steps, 0.0);

  // Cumulative losses for each step.
  std::vector<double> losses;
  losses.resize(n_steps, 0.0);

  Action action{};
  double reward{};

  // Start the training process
  for (size_t n = 0; n < n_episodes; ++n) {

    // Sample the environment for the designated number of steps.
    for (size_t s = 0; s < n_steps; ++s) {
      std::tie(action, reward) = agent.sample();

      // We collect rewards and losses for each step.
      rewards[s] += reward;
      losses[s] += loss(bandit, action);
    }

    // Reset the agent's memory in preparation for the next round.
    agent.reset(epsilon);
  }

  // Make each entry equal to the average over the episodes
  for (size_t s = 0; s < n_steps; ++s) {
    rewards[s] /= n_episodes;
    losses[s] /= n_episodes;
  }

  // Log results
  json j;
  j["n_actions"] = n_actions;
  j["epsilon"] = epsilon;
  j["n_steps"] = n_steps;
  j["n_episodes"] = n_episodes;
  j["rewards"] = json::array({});
  for (auto r : rewards) {
    j["rewards"].push_back(r);
  }
  j["losses"] = json::array({});
  for (auto l : losses) {
    j["losses"].push_back(l);
  }

  json_log(make_log_fn(n_actions, epsilon, n_steps, n_episodes), std::move(j));

  return EXIT_SUCCESS;
}
