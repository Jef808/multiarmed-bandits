#ifndef JSONLOG_H_
#define JSONLOG_H_

#include <string_view>
#include <vector>

namespace stats {
struct Store;
}

namespace jsonlog {
void log(std::string_view policy_name, const stats::Store &store,
         std::string_view output_file);
}

#endif // JSONLOG_H_
