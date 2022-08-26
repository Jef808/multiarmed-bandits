#include "jsonlog.h"
#include "po_helper.h"

#include <nlohmann/json.hpp>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace jsonlog {


void log(std::string_view name, std::vector<double> &&rewards,
         std::vector<double> &&losses) {
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

  std::ofstream ofs{ vm["output-file"].as<std::string>() };
  if (not ofs) {
    std::cerr << vm["output-file"].as<std::string>();
    throw std::runtime_error("Unable to open output file");
  }
  try {
    ofs << j.dump() << std::endl;
    std::cout << "Wrote output to log file " << vm["output-file"].as<std::string>() << std::endl;
  } catch (std::exception &e) {
    std::cerr << e.what();
    throw std::runtime_error("Error while dumping json object to output file");
  }
}

} // namespace jsonlog
