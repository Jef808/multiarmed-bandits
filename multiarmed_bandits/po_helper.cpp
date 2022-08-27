#include "po_helper.h"

#include <boost/program_options.hpp>

#include <algorithm>
#include <iostream>

// constexpr auto ROOT = "/home/jfa/projects/ai/multiarmed_bandits/config.cfg";

template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v) {
  std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, " "));
  return out;
}

namespace po = boost::program_options;

namespace parseopts {

po::variables_map vm;

void on_configfile(std::string_view configfile) {
  std::cout << "Config file set to " << configfile << '\n';
}
void on_actions(int age) { std::cout << "Actions set to " << age << '\n'; }
void on_episodes(int episodes) {
  std::cout << "Episodes set to " << episodes << '\n';
}
void on_stepsize(int stepsize) {
  std::cout << "Step size set to " << stepsize << '\n';
}
void on_steps(int steps) {
  std::cout << "Number of steps set to " << steps << '\n';
}
void on_exploration(double exploration) {
  std::cout << "Exploration constant set to " << exploration << '\n';
}
void on_outputfile(const std::vector<std::string> &outputfile) {
  std::cout << "Output file: " << outputfile << '\n';
}

bool parse(int argc, char *argv[]) {

  // To declare default values
  std::string config_file = DEFAULT_CONFIG_FILEPATH;

  // Group of options only available on command line
  po::options_description generic("Generic options");
  generic.add_options()("help,h", "show usage message")(
      "debug,d", "set debug mode")("config,f",
                                   po::value<std::string>(&config_file)
                                       ->default_value(DEFAULT_CONFIG_FILEPATH)
                                       ->notifier(on_configfile),
                                   "set config file");

  // Group of options allowed both on command line
  // and in config file
  po::options_description config("Configuration");
  config.add_options()("actions,a",
                       po::value<int>()
                           // ->default_value(10)
                           ->notifier(on_actions),
                       "set number of actions")("episodes,n",
                                                po::value<int>()
                                                    // ->default_value(500)
                                                    ->notifier(on_episodes),
                                                "set number of episodes")(
      "stepsize,j",
      po::value<int>()
          // ->default_value(10)
          ->notifier(on_stepsize),
      "set length of the step size")("steps,s",
                                     po::value<int>()
                                         // ->default_value(500)
                                         ->notifier(on_steps),
                                     "set number of steps")(
      "exploration,c",
      po::value<double>()
          // ->default_value(0.4)
          ->notifier(on_exploration),
      "set exploration constant");

  // Hidden options
  po::options_description hidden("Hidden options");
  hidden.add_options()(
      "output-file",
      po::value<std::vector<std::string>>()->composing()->notifier(
          on_outputfile),
      "output file");

  // We further group the options
  po::options_description cmdline_options;
  cmdline_options.add(generic).add(config).add(hidden);

  po::options_description config_file_options;
  config_file_options.add(config);

  po::options_description visible;
  visible.add(generic).add(config);

  // Positional options shall be grouped into "output-file"
  po::positional_options_description p;
  p.add("output-file", -1);

  // Will store the parsed values of the program options.
  // Note that the first call might modify the value of "config_file"
  po::store(po::command_line_parser(argc, argv)
                .options(cmdline_options)
                .positional(p)
                .run(),
            vm);
  po::store(po::parse_config_file(config_file.data(), config_file_options), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << "Usage: " << argv[0] << " [OPTIONS]... [OUTPUT]...\n"
              << visible << "\n";
    return false;
  }

  return true;
}

} // namespace parseopts
