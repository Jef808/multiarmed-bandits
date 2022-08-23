#include "agent.h"
#include "multiarmed_bandits.h"
#include "policies.h"

#include <algorithm>
#include <array>
#include <cassert>
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

constexpr double default_exploration_constant = 2;
constexpr size_t default_n_actions = 10;
constexpr size_t default_n_steps = 1000;
constexpr size_t default_n_episodes = 2000;
constexpr double default_debug = false;
constexpr double default_help = false;

const std::string log_dir = "/home/jfa/projects/ai/logs/multiarmed_bandits/";

using ExtAction = ExtActionT<Action>;

inline double loss(const NArmedBandit &bandit, const Action &chosen_action) {
  return bandit.expectation(bandit.best_action()) -
         bandit.expectation(chosen_action);
}

constexpr size_t COL_MAX = 32;

template <typename OutputIter>
OutputIter help_line(OutputIter out, const char o_short,
                     std::string_view o_long, std::string_view desc,
                     std::string_view o_param = "",
                     bool o_param_mandatory = false) {
  if (o_param_mandatory && o_param.empty()) {
    const std::string msg =
        "help_line(): o_param_mandatory must be false if o_param is empty";
    throw std::invalid_argument(msg);
  }

  std::stringstream ss;
  auto _out_beg = std::ostreambuf_iterator<char>(ss);
  auto [_out, sz] = fmt::format_to_n(_out_beg, COL_MAX + 1,
                                     "  -{0}, --{1}{2}{3}{4}", o_short, o_long,
                                     o_param.empty()     ? ""
                                     : o_param_mandatory ? "="
                                                         : "[=",
                                     o_param,
                                     o_param.empty()     ? ""
                                     : o_param_mandatory ? ""
                                                         : "]");

  if (sz >= COL_MAX) {
    const std::string msg =
        fmt::format("help_line(): Formatted string before description must "
                    "have size at most {0}",
                    COL_MAX);
    throw std::length_error(msg);
  }

  fmt::format_to(out, "{0}", ss.str());
  std::string_view sv = desc;
  while (not sv.empty()) {
    size_t n_spaces = COL_MAX - sz;
    std::string spaces = std::string(n_spaces, ' ');
    std::copy(spaces.begin(), spaces.end(), out);
    std::string_view nsv = sv.substr(0, COL_MAX);
    auto n = nsv == sv? sv.size(): nsv.find_last_of(" \0");
    std::copy_n(nsv.begin(), n-1, out);
    sv.remove_prefix(n);
    out = '\n';
    sz = -1;
  }

  return out;
}

void print_usage(std::string_view prog_name) {
  auto out = std::ostream_iterator<char>(std::cout);

  out = fmt::format_to(out, "Usage: {0} [OPTIONS]...\n{1}\n\n{2}\n", prog_name,
                       "Sample a Multi-armed-bandit (MAB) environment using an "
                       "Upper Confidence Bound (UCB) policy.",
                       "Mandatory arguments to long options are mandatory for "
                       "short options too.");

  out = help_line(out, 'a', "actions", "Number of actions for the MAB's", "N",
                  true);

  out = help_line(out, 'c', "exploration", "Exploration constant", "C", true);

  out = help_line(out, 's', "steps", "Number of steps", "N", true);

  out = help_line(out, 's', "episodes", "Number of episodes", "N", true);

  out = help_line(out, 'd', "debug", "Debug flag", "", false);

  out = help_line(out, 'h', "help", "Print this message", "", false);
}

bool parse_opts(int argc, char *argv[], size_t &n_actions,
                double &exploration_constant, size_t &n_steps,
                size_t &n_episodes, bool &debug, bool &help) {

  std::set<std::string_view> seen;
  std::vector<std::string_view> args;
  for (auto i = 0; i < argc; ++i) {
    args.push_back(argv[i]);
  }

  auto it = std::find_if(args.begin(), args.end(),
                         [](auto arg) { return arg[0] == '-'; });
  while (it != args.end()) {
    char c = (*it)[1];
    try {
      switch (c) {
      case 'a':
        if (seen.count("actions")) {
          throw std::invalid_argument("Duplicate program options");
        }
        n_actions = std::stoi((it + 1)->data());
        fmt::print("Set number of actions to {0}\n", n_actions);
        seen.insert("actions");
        break; // number of actions
      case 'c':
        if (seen.count("exploration")) {
          throw std::invalid_argument("Duplicate program options");
        }
        exploration_constant = std::stod((it + 1)->data());
        fmt::print("Set exploration_constant to {0}\n", exploration_constant);
        break; // epsilon
      case 's':
        if (seen.count("steps")) {
          throw std::invalid_argument("Duplicate program options");
        }
        n_steps = std::stoi((it + 1)->data());
        fmt::print("Set number of steps to {0}\n", n_steps);
        break; // number of steps
      case 'n':
        if (seen.count("episodes")) {
          throw std::invalid_argument("Duplicate program options");
        }
        n_episodes = std::stoi((it + 1)->data());
        fmt::print("Set number of episodes to {0}\n", n_episodes);
        break; // number of episodes
      case 'd':
        if (not seen.insert("debug").second) {
          throw std::invalid_argument("Duplicate program options");
        }
        debug = true;
        fmt::print("Set debug flag on\n", n_episodes);
        break; // debug
      case 'h':
        if (not seen.insert("help").second) {
          throw std::invalid_argument("Duplicate program options");
        }
        help = true;
        fmt::print("Set help flag on\n", n_episodes);
        break; // number of episodes
      case '-': {
        std::string_view opt = it->substr(2);
        if (opt == "debug") {
          if (not seen.insert("debug").second)
            throw std::invalid_argument("Duplicate program options");
          debug = true;
        } else if (opt == "help") {
          if (not seen.insert("help").second)
            throw std::invalid_argument("Duplicate program options");
          help = true;
        } else if (opt == "actions") {
          if (not seen.insert("actions").second)
            throw std::invalid_argument("Duplicate program options");
          n_actions = std::stoi((it + 1)->data());
          fmt::print("Set number of actions to {0}\n", n_actions);
        } else if (opt == "exploration") {
          bool err = not seen.insert("exploration").second;
          if (err) {
            throw std::invalid_argument("Duplicate program options");
          }
          exploration_constant = std::stod((it + 1)->data());
          fmt::print("Set exploration_constant to {0}\n", exploration_constant);
        } else if (opt == "steps") {
          if (seen.count("steps")) {
            throw std::invalid_argument("Duplicate program options");
          }
          n_steps = std::stoi((it + 1)->data());
          fmt::print("Set number of steps to {0}\n", n_steps);
          seen.insert("steps");
        } else if (opt == "episodes") {
          if (seen.count("episodes")) {
            throw std::invalid_argument("Duplicate program options");
          }
          n_episodes = std::stoi((it + 1)->data());
          fmt::print("Set number of episodes to {0}\n", n_episodes);
          seen.insert("episodes");
        }
      } break;
      default:
        fmt::print("Unrecognized command line argument.");
        break;
      }
    } catch (std::exception &e) {
      fmt::print("Invalid input parameter: {0}\n", e.what());
      return false;
    }
    it = std::find_if(it + 1, args.end(),
                      [](auto arg) { return arg[0] == '-'; });
  }
  return true;
}

std::string make_log_fn(std::string_view policy_name, int n_actions,
                        int n_episodes, int n_steps, double exp_cst) {
  int exp_cst_100 = static_cast<int>(100.0 * exp_cst);
  std::stringstream ss;
  ss << log_dir << policy_name << '_' << n_actions << '_' << n_episodes << '_'
     << n_steps << '_' << exp_cst_100 << ".json";
  return ss.str();
}

bool json_log(nlohmann::json &&j) {
  using namespace nlohmann;
  json json = j;
  std::string policy_name = json["policy"].get<json::string_t>();
  std::uint64_t n_actions = json["n_actions"].get<json::number_unsigned_t>();
  std::uint64_t n_steps = json["n_steps"].get<json::number_unsigned_t>();
  std::uint64_t n_episodes = json["n_episodes"].get<json::number_unsigned_t>();
  double exp_cst = json["exploration_constant"].get<json::number_float_t>();

  std::string fn =
      make_log_fn(policy_name, n_actions, n_episodes, n_steps, exp_cst);
  std::ofstream ofs{fn};
  if (not ofs) {
    fmt::print("WARNING: Unable to open log file: {0}\n", fn);
  }

  try {
    ofs << j.dump(4) << std::endl;
    fmt::print("Wrote output to log file {0}\n", fn);
    return true;
  } catch (std::exception &e) {
    std::cerr << "Failed to dump json object to string: {0}" << e.what()
              << std::endl;
    return false;
  }
}

int main(int argc, char *argv[]) {
  using namespace nlohmann;

  size_t n_actions = default_n_actions;
  double exploration_constant = default_exploration_constant;
  size_t n_steps = default_n_steps;
  size_t n_episodes = default_n_episodes;
  bool debug = default_debug;
  bool help = default_help;

  parse_opts(argc, argv, n_actions, exploration_constant, n_steps, n_episodes,
             debug, help);

  if (help) {
    print_usage(argv[0]);
    return EXIT_SUCCESS;
  }

  NArmedBandit bandit{n_actions};
  Agent<Policy_UCB> agent(bandit, Policy_UCB{exploration_constant});

  // Cumulative rewards for each step.
  std::vector<double> rewards;
  rewards.resize(n_steps, 0.0);

  // Cumulative losses for each step.
  std::vector<double> losses;
  losses.resize(n_steps, 0.0);

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

  // Start} the training process
  for (size_t n = 0; n < n_episodes; ++n) {

    // Sample the environment for the designated number of steps.
    for (size_t s = 0; s < n_steps; ++s) {
      std::tie(action, reward) = agent.sample();

      // We collect rewards and losses for each step.
      rewards[s] += reward;
      losses[s] += loss(bandit, action);

      if (debug) {
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
  for (size_t s = 0; s < n_steps; ++s) {
    rewards[s] /= n_episodes;
    losses[s] /= n_episodes;
  }

  // Log results
  json j;
  j["policy"] = "ucb";
  j["n_actions"] = n_actions;
  j["exploration_constant"] = exploration_constant;
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

  bool log_okay = json_log(std::move(j));

  return log_okay ? EXIT_SUCCESS : EXIT_FAILURE;
}
