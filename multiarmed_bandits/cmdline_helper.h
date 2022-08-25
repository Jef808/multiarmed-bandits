#ifndef CMDLINE_HELPER_H_
#define CMDLINE_HELPER_H_

#include <algorithm>
#include <array>
#include <cassert>
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

constexpr size_t COL_MAX = 32;

const std::string ROOTDIR = "/home/jfa/projects/ai/";
struct Options {
  size_t n_actions;
  double exploration_constant;
  size_t stepsize;
  size_t n_steps;
  size_t n_episodes;
  std::string output;
  bool debug;
  bool help;
};


inline void help_line(std::ostream_iterator<char>& out, const char o_short,
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
    auto n = nsv == sv ? sv.size() : nsv.find_last_of(" \0");
    std::copy_n(nsv.begin(), n, out);
    sv.remove_prefix(n);
    out = '\n';
    sz = -1;
  }
}

inline void print_usage(std::string_view prog_name) {
  auto out = std::ostream_iterator<char>(std::cout);

  fmt::format_to(out, "Usage: {0} [OPTIONS]...\n{1}\n\n{2}\n", prog_name,
                       "Sample a Multi-armed-bandit (MAB) environment using an "
                       "Upper Confidence Bound (UCB) policy.",
                       "Mandatory arguments to long options are mandatory for "
                       "short options too.");

  help_line(out, 'a', "actions", "Number of actions for the MAB's", "N",
                  true);

  help_line(out, 'c', "exploration", "Exploration constant", "C", true);

  help_line(out, 's', "steps", "Number of steps", "N", true);
  help_line(out, 'o', "output", "Output filename", "FILE", true);

  help_line(out, 'j', "stepsize", "Step size", "SIZE", true);
  help_line(out, 'n', "episodes", "Number of episodes", "N", true);

  help_line(out, 'd', "debug", "Debug flag", "", false);

  help_line(out, 'h', "help", "Print this message", "", false);
}


inline bool parse_opts(int argc,
                char *argv[],
                Options& opts) {

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
        opts.n_actions = std::stoi((it + 1)->data());
        fmt::print("Set number of actions to {0}\n", opts.n_actions);
        seen.insert("actions");
        break; // number of actions
      case 'c':
        if (seen.count("exploration")) {
          throw std::invalid_argument("Duplicate program options");
        }
        opts.exploration_constant = std::stod((it + 1)->data());
        fmt::print("Set exploration_constant to {0}\n", opts.exploration_constant);
        seen.insert("exploration");
        break; // epsilon
      case 's':
        if (seen.count("steps")) {
          throw std::invalid_argument("Duplicate program options");
        }
        opts.n_steps = std::stoi((it + 1)->data());
        fmt::print("Set number of steps to {0}\n", opts.n_steps);
        seen.insert("steps");
        break; // number of steps
      case 'j':
        if (seen.count("stepsize")) {
          throw std::invalid_argument("Duplicate program options");
        }
        opts.stepsize = std::stoi((it + 1)->data());
        fmt::print("Set step size to {0}\n", opts.stepsize);
        seen.insert("stepsize");
        break; // step size
      case 'n':
        if (seen.count("episodes")) {
          throw std::invalid_argument("Duplicate program options");
        }
        opts.n_episodes = std::stoi((it + 1)->data());
        fmt::print("Set number of episodes to {0}\n", opts.n_episodes);
        seen.insert("episodes");
        break; // number of episodes
      case 'd':
        if (not seen.insert("debug").second) {
          throw std::invalid_argument("Duplicate program options");
        }
        opts.debug = true;
        fmt::print("Set debug flag {0}\n", opts.debug? "on": "off");
        break; // debug
      case 'h':
        if (not seen.insert("help").second) {
          throw std::invalid_argument("Duplicate program options");
        }
        opts.help = true;
        fmt::print("Set help flag {0}\n", opts.help? "on": "off");
        break; // number of episodes
      case '-': {
        std::string_view opt = it->substr(2);
        if (opt == "debug") {
          if (not seen.insert("debug").second)
            throw std::invalid_argument("Duplicate program options");
          opts.debug = true;
        } else if (opt == "help") {
          if (not seen.insert("help").second)
            throw std::invalid_argument("Duplicate program options");
          opts.help = true;
        } else if (opt == "actions") {
          if (not seen.insert("actions").second)
            throw std::invalid_argument("Duplicate program options");
          opts.n_actions = std::stoi((it + 1)->data());
          fmt::print("Set number of actions to {0}\n", opts.n_actions);
        } else if (opt == "exploration") {
          bool err = not seen.insert("exploration").second;
          if (err) {
            throw std::invalid_argument("Duplicate program options");
          }
          opts.exploration_constant = std::stod((it + 1)->data());
          fmt::print("Set exploration_constant to {0}\n", opts.exploration_constant);
        } else if (opt == "steps") {
          if (seen.count("steps")) {
            throw std::invalid_argument("Duplicate program options");
          }
          opts.n_steps = std::stoi((it + 1)->data());
          fmt::print("Set number of steps to {0}\n", opts.n_steps);
          seen.insert("steps");
        } else if (opt == "stepsize") {
          if (not seen.insert("stepsize").second)
            throw std::invalid_argument("Duplicate program options");
          opts.stepsize = std::stoi((it + 1)->data());
          fmt::print("Set step size to {0}\n", opts.stepsize);
        } else if (opt == "episodes") {
          if (seen.count("episodes")) {
            throw std::invalid_argument("Duplicate program options");
          }
          opts.n_episodes = std::stoi((it + 1)->data());
          fmt::print("Set number of episodes to {0}\n", opts.n_episodes);
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


#endif // CMDLINE_HELPER_H_
