#include "agent.h"
#include "multiarmed_bandits.h"
#include "policies.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

constexpr double default_epsilon = 0.1;
constexpr size_t default_n_actions = 10;
constexpr size_t default_n_steps = 1000;
constexpr size_t default_stepsize = 1;
constexpr size_t default_n_episodes = 2000;

const std::string log_dir = "/home/jfa/projects/ai/logs/multiarmed_bandits/";

using ExtAction = ExtActionT<Action>;

struct Options {
  size_t n_actions{ default_n_actions };
  double epsilon {default_epsilon};
  size_t stepsize {default_stepsize};
  size_t n_steps {default_n_steps};
  size_t n_episodes {default_n_episodes};
  // bool debug {default_debug};
  // bool help {default_help};
};

inline double loss(const NArmedBandit &bandit, const Action &chosen_action) {
  return bandit.expectation(bandit.best_action()) -
         bandit.expectation(chosen_action);
}

bool parse_opts(int argc, char *argv[], Options& opts) {

  auto& n_actions = opts.n_actions;
  auto& epsilon = opts.epsilon;
  auto& stepsize = opts.stepsize;
  auto& n_steps = opts.n_steps;
  auto& n_episodes = opts.n_episodes;

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
      case 'j':
        stepsize = std::stoi((it + 1)->data());
        fmt::print("Set step size to {0}\n", stepsize);
        break; // step size
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

std::string make_log_fn(std::string_view policy_name, const Options& opts) {
  int epsilon_100 = static_cast<int>(100.0 * opts.epsilon);
  std::stringstream ss;
  ss << log_dir << policy_name << '_' << opts.n_actions << '_' << opts.n_episodes << '_'
     << opts.n_steps << '_' << opts.stepsize << '_' << epsilon_100 << ".json";
  return ss.str();
}

void json_log(nlohmann::json &&j) {
  using namespace nlohmann;
  json json = j;
  Options opts{};
  std::string policy_name = json["policy"].get<json::string_t>();
  opts.n_actions = json["n_actions"].get<json::number_unsigned_t>();
  opts.n_steps = json["n_steps"].get<json::number_unsigned_t>();
  opts.stepsize = json["stepsize"].get<json::number_unsigned_t>();
  opts.n_episodes = json["n_episodes"].get<json::number_unsigned_t>();
  opts.epsilon = json["epsilon"].get<json::number_float_t>();

  std::string fn = make_log_fn(policy_name, opts);
  std::ofstream ofs{ fn };
  if (not ofs) {
    fmt::print("WARNING: Unable to open log file: {0}\n", fn);
  }

  ofs << j.dump(4) << std::endl;
  fmt::print("Wrote output to log file {0}\n", fn);
}

int main(int argc, char *argv[]) {
  using namespace nlohmann;

  Options opts;

  size_t& n_actions = opts.n_actions = default_n_actions;
  double& epsilon = opts.epsilon = default_epsilon;
  size_t& stepsize = opts.stepsize = default_stepsize;
  size_t& n_steps = opts.n_steps = default_n_steps;
  size_t& n_episodes = opts.n_episodes = default_n_episodes;

  parse_opts(argc, argv, opts);

  NArmedBandit bandit{n_actions};
  Agent< Policy_Greedy > agent(bandit, Policy_Greedy{epsilon});
  //AgentGreedy agent{bandit, epsilon};

  // Cumulative rewards for each step.
  std::vector<double> rewards;
  rewards.resize(n_steps, 0.0);

  // Cumulative losses for each step.
  std::vector<double> losses;
  losses.resize(n_steps, 0.0);

  // int total_visits;
  // std::vector<int> action_visits;
  // std::vector<double> action_values;
  // std::vector<double> action_pvalues;
  std::vector<double> action_qvalues;
  std::generate_n(std::back_inserter(action_qvalues),
                  bandit.number_of_actions(), [&bandit, n = 0]() mutable {
                    return bandit.expectation(Action{static_cast<size_t>(n++)});
                  });

  Action action{};
  double reward{};

  // Start the training process
  for (size_t n = 0; n < n_episodes; ++n) {

    // Sample the environment for the designated number of steps.
    for (size_t s = 0; s < n_steps; ++s) {

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
  j["stepsize"] = stepsize;
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
