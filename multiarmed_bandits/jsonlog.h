#ifndef JSONLOG_H_
#define JSONLOG_H_

#include <string_view>
#include <vector>

namespace jsonlog {
void log(std::string_view name, std::vector<double> &&rewards,
         std::vector<double> &&losses, std::string_view output_file);
}

#endif // JSONLOG_H_
