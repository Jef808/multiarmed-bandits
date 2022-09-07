#include "jsonlog.h"
#include "po_helper.h"
#include "statistics.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace jsonlog {

template <stats::Type T>
inline void stats_to_json(const std::string &name,
                          const typename stats::Traits<T>::data_type &data,
                          nlohmann::json &j) {
  using namespace nlohmann;

  if constexpr (T == stats::Type::Series) {
    j[name] = json::array({});
    for (auto s : data) {
      j[name].push_back(s);
    }
  } else if constexpr (T == stats::Type::WideSeries) {
    j[name] = nlohmann::json::array({});
    for (const auto &step : data) {
      auto &jarr = j[name].emplace_back(nlohmann::json::array({}));
      for (auto s : step) {
        jarr.push_back(s);
      }
    }
  }
}

void log(std::string_view policy_name, const stats::Store &store,
         std::string_view output_file) {
  using namespace nlohmann;
  using parseopts::vm;

  json j;

  for (auto it = store.points_beg(); it != store.points_end(); ++it) {
    j[it->first] = it->second;
    // stats_to_json<stats::Type::Point>(it->first, it->second, j);
  }
  for (auto it = store.series_beg(); it != store.series_end(); ++it) {
    j[it->first] = it->second;
    // stats_to_json<stats::Type::Series>(it->first, it->second, j);
  }
  for (auto it = store.wide_series_beg(); it != store.wide_series_end(); ++it) {
    j[it->first] = it->second;
    // stats_to_json<stats::Type::RunningSeries>(it->first, it->second, j);
  }

  j["policy"] = policy_name;
  j["n_actions"] = vm["actions"].as<int>();
  j["exploration_constant"] = vm["exploration"].as<double>();
  j["n_steps"] = vm["steps"].as<int>();
  j["stepsize"] = vm["stepsize"].as<int>();
  j["n_episodes"] = vm["episodes"].as<int>();

  std::ofstream ofs{output_file.data()};
  ofs << j.dump() << std::endl;
}

} // namespace jsonlog
