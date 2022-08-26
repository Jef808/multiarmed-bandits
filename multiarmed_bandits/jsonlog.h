#ifndef __JSONLOG_H
#define __JSONLOG_H

#include <string_view>
#include <vector>

namespace jsonlog {
    void log(std::string_view name, std::vector<double> &&rewards,
             std::vector<double> &&losses);
}

#endif
