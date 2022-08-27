#include "jsonlog.h"
#include "po_helper.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace jsonlog {

void log(std::string_view name, std::vector<double> &&rewards,
         std::vector<double> &&losses, std::string_view output_file) {
  using namespace nlohmann;
  using parseopts::vm;

  json j;

  std::vector<double> _rewards = rewards;
  std::vector<double> _losses = losses;

  j["policy"] = name;
  j["n_actions"] = vm["actions"].as<int>();
  j["exploration_constant"] = vm["exploration"].as<double>();
  j["n_steps"] = vm["steps"].as<int>();
  j["stepsize"] = vm["stepsize"].as<int>();
  j["n_episodes"] = vm["episodes"].as<int>();
  j["rewards"] = json::array({});
  j["losses"] = json::array({});
  for (auto r : _rewards) {
    j["rewards"].push_back(r);
  }
  for (auto l : _losses) {
    j["losses"].push_back(l);
  }

  std::ofstream ofs{output_file.data()};

  ofs << j.dump() << std::endl;
  // if (not ofs) {
  //   auto msg = "Unable to open output file: " + std::string(output_file);
  //   throw std::runtime_error(msg);
  // }
  // try {

  // } catch (std::exception &e) {
  //   auto msg = "Error while dumping json to file " + std::string(output_file)
  //   +
  //              ": " + e.what();
  //   throw std::runtime_error(msg);
  // }
}

} // namespace jsonlog
