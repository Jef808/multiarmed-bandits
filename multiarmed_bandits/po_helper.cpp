#include "po_helper.h"

#include <iostream>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace parseopts {

po::variables_map vm;


bool parse(int argc, char* argv[]) {

    int da, dn, dj, ds;
    double dc;

  // Group of options only available on command line
  po::options_description generic("Generic options");
  generic.add_options()
    ("help,h", "show usage message")
    ("debug,d", "step through the run with verbose outputs")
    ;

  // Group of options allowed both on command line
  // and in config file
  po::options_description config("Configuration");
  config.add_options()
    ("actions,a", po::value<int>(&da)->default_value(10), "set number of actions")
    ("episodes,n", po::value<int>(&dn)->default_value(500), "set number of episodes")
    ("stepsize,j", po::value<int>(&dj)->default_value(10), "set length of the step size")
    ("steps,s", po::value<int>(&ds)->default_value(500), "set number of steps")
    ("exploration,c", po::value<double>(&dc)->default_value(0.4), "set exploration constant")
    ;

  // Hidden options
  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("output-file", po::value< std::vector< std::string > >()->composing(), "output file")
    ;

  // We further group the options
  po::options_description cmdline_options;
  cmdline_options.add(generic).add(config).add(hidden);

  po::options_description config_file_options;
  config_file_options.add(config);

  po::options_description visible;
  visible.add(generic).add(config);

  // Positional options shall be grouped into "output-file"
  po::positional_options_description p;
  p.add("output-file", 1);

  // Will store the parsed values of the program options
  po::store(po::parse_config_file("/home/jfa/projects/ai/multiarmed_bandits/config.cfg",
                                  config_file_options), vm);
  po::store(po::command_line_parser(argc, argv)
            .options(cmdline_options)
            .positional(p)
            .run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
      std::cout << "Usage: " << argv[0] << " [OPTIONS]... [OUTPUT]...\n"
                << visible << "\n";
      return false;
  }

  return true;
}


} // namespace parseopts
