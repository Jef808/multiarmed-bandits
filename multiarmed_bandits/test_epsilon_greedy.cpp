#include "agent.h"
#include "multiarmed_bandits.h"
#include "policies.h"

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

std::string make_log_fn(std::string_view policy_name, int n_actions,
                        int n_episodes, int n_steps, double epsilon) {
  int epsilon_100 = static_cast<int>(100.0 * epsilon);
  std::stringstream ss;
  ss << log_dir << policy_name << '_' << n_actions << '_' << n_episodes << '_'
     << n_steps << '_' << epsilon_100 << ".json";
  return ss.str();
}

void json_log(nlohmann::json &&j) {
  using namespace nlohmann;
  json json = j;
  std::string policy_name = json["policy"].get<json::string_t>();
  std::uint64_t n_actions = json["n_actions"].get<json::number_unsigned_t>();
  std::uint64_t n_steps = json["n_steps"].get<json::number_unsigned_t>();
  std::uint64_t n_episodes = json["n_episodes"].get<json::number_unsigned_t>();
  double epsilon = json["epsilon"].get<json::number_float_t>();

  std::string fn = make_log_fn(policy_name, n_actions, n_episodes, n_steps, epsilon);
  std::ofstream ofs{ fn };
  if (not ofs) {
    fmt::print("WARNING: Unable to open log file: {0}\n", fn);
  }

  ofs << j.dump(4) << std::endl;
  fmt::print("Wrote output to log file {0}\n", fn);
}

int main(int argc, char *argv[]) {
  using namespace nlohmann;

  size_t n_actions = default_n_actions;
  double epsilon = default_epsilon;
  size_t n_steps = default_n_steps;
  size_t n_episodes = default_n_episodes;

  parse_opts(argc, argv, n_actions, epsilon, n_steps, n_episodes);

  NArmedBandit bandit{n_actions};
  Agent< Policy_Greedy > agent(bandit, Policy_Greedy{epsilon});
  //AgentGreedy agent{bandit, epsilon};

  // Cumulative rewards for each step.
  std::vector<double> rewards;
  rewards.resize(n_steps, 0.0);

  // Cumulative losses for each step.
  std::vector<double> losses;
  losses.resize(n_steps, 0.0);

  Action action{};
  double reward{};

  // int total_visits;
  // std::vector<int> action_visits;
  // std::vector<double> action_values;
  // std::vector<double> action_pvalues;
  std::vector<double> action_qvalues;
  std::generate_n(std::back_inserter(action_qvalues),
                  bandit.number_of_actions(), [&bandit, n = 0]() mutable {
                    return bandit.expectation(Action{static_cast<size_t>(n++)});
                  });

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
    agent.reset();
  }

  // Make each entry equal to the average over the episodes
  for (size_t s = 0; s < n_steps; ++s) {
    rewards[s] /= n_episodes;
    losses[s] /= n_episodes;
  }

  // Log results
  json j;
  j["policy"] = "epsilon_greedy";
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
  j["true_values"] = json::array({});
  for (auto v : action_qvalues) {
    j["true_values"].push_back(v);
  }

  json_log(std::move(j));

  return EXIT_SUCCESS;
}
