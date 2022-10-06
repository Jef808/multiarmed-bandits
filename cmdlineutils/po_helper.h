#ifndef PO_HELPER_H_
#define PO_HELPER_H_

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

namespace parseopts {

extern boost::program_options::variables_map vm;

bool parse(int argc, char *argv[]);

} // namespace parseopts

#endif // PO_HELPER_H_
