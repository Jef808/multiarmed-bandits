#ifndef __PO_HELPERS_H
#define __PO_HELPERS_H

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

namespace parseopts {

extern boost::program_options::variables_map vm;

bool parse(int argc, char* argv[]);

} // namespace parseopts

#endif
