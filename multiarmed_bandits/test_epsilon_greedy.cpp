#include "agent_greedy.h"
#include "multiarmed_bandits.h"

#include <array>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/core.h>
#include <nlohmann/json.hpp>

constexpr double default_epsilon = 0.1;
constexpr size_t default_n_actions = 10;
constexpr size_t default_n_steps = 1000;
constexpr size_t default_n_episodes = 2000;

Action highest_expectation(const NArmedBandit &bandit) {
    Action best_a{0};
    double best_v{bandit.expectation(best_a)};

    for (size_t i = 1; i < bandit.number_of_actions(); ++i) {
        Action action{i};
        if (double expec_v = bandit.expectation(action);
            expec_v > best_v)
        {
            best_a = action;
            best_v = expec_v;
        }
    }
    return best_a;
}

/**
 * Accumulate the difference in expectations between the best choices and
 * the choices that were made.
 */
inline double total_loss(const NArmedBandit& bandit, const std::vector<ExtAction>& eas) {
    double max_val = bandit.expectation(bandit.best_action());
    return std::accumulate(eas.begin(), eas.end(), 0.0, [&bandit, max_val](double& s, const auto& ea){
        return s += ea.visits * (max_val - bandit.expectation(ea.action));
    });
}

/**
 * Accumulate the rewards from each choices made.
 */
inline double total_val(const std::vector< ExtAction >& eas, const double prior=0.0) {
    return std::accumulate(eas.begin(), eas.end(), 0.0, [](double& s, const ExtAction& ea){
        return s += ea.total;
    });
}

int main(int argc, char *argv[]) {
  using namespace nlohmann;

  size_t n_actions = 10;
  double epsilon = 0.1;
  size_t n_steps = 1000;
  size_t n_episodes = 2000;

  std::vector<std::string_view> args;
  for (auto i = 0; i < argc; ++i) {
      args.push_back(argv[i]);
  }

  for (auto cc : { "-a", "-e", "-s", "-N"}) {
      auto it = std::find(args.begin(), args.end(), cc);
      if (it != args.end()) {
          switch ((*it)[1]) {
              case 'a': n_actions = std::stoi((it+1)->data()); break;
              case 'e': epsilon = std::stod((it+1)->data()); break;
              case 's': n_steps = std::stoi((it+1)->data()); break;
              case 'N': n_episodes = std::stoi((it+1)->data()); break;
              default: break;
          }
      }
  }

  NArmedBandit bandit{n_actions};
  AgentGreedy agent{bandit, epsilon};

  // Initialize buffers for capturing snapshots of
  // the learning agents.
  std::vector< std::vector< ExtAction > > snapshots;
  for (size_t i = 0; i < n_episodes; ++i) {
      auto& eas = snapshots.emplace_back();
      for (size_t a = 0; a < n_actions; ++a) {
          eas.emplace_back(a);
      }
  }

  // Get the maximal reward expectation to compute a loss function.
  double value_best_action = bandit.expectation(bandit.best_action());

  // To vizualize the evolution of the learning process.
  double cumulative_total = 0.0;
  double cumulative_loss = 0.0;

  json j = json::array();

  // Start the training process
  for (size_t n = 0; n < n_episodes; ++n)
  {
      // Let the agent sample rewards for one episode
      // and extract the statistics generated.
      agent.sample(snapshots[n], n_steps);

      json jarray = json::array();

      for (size_t a = 0; a < n_actions; ++a)
      {
          assert(snapshots[n][a].action.idx == a
                 && "snapshots' action indices out of order");

          json ja;
          ja["action"] = a;

          int visits = snapshots[n][a].visits;
          double total = snapshots[n][a].total;
          double value = bandit.expectation(Action{a});
          double loss = visits * (value_best_action - value);

          cumulative_total += total;
          cumulative_loss += loss;

          ja["visits"] = visits;
          ja["average"] = total/visits;
          ja["expected_value"] = value;
          ja["loss"] = loss;

          jarray.push_back(ja);
          // std::cout << std::setprecision(2)
          //           << "\n\033[1mAction\033[0m " << a
          //           << ": Visits = " << visits
          //           << ", Avg = " << (visits > 0? total/visits: 0.0)
          //           << ", Expected = " << value
          //           << ", Loss = " << loss << std::endl;
      }

      // json jepisode;
      // jepisode["episode"] = n;
      // jepisode.push_back(jarray);
      json ep = json::array({ {"episode", n}, jarray });
      j.push_back(ep);
      // std::cout << "  \033[1mTotal avg reward\033[0m = "
      //           << cumulative_total / (n * n_steps)
      //           << ", \033[1mTotal loss\033[0m = "
      //           << cumulative_loss << std::endl;
  }

  std::ofstream ofs{ "/home/jfa/projects/ai/greedy.json" };
  ofs << j.dump(4) << std::endl;

  // for (size_t i = 0; i < default_n_actions; ++i) {
  //     std::cout << "Action " << i << ": " << "Average loss = " << results[i] << std::endl;
  // }


  return EXIT_SUCCESS;
}
