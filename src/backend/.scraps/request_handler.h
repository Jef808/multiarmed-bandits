#ifndef REQUEST_HANDLER_H_
#define REQUEST_HANDLER_H_

#include "serialized.h"
#include <string>
#include <vector>

namespace Request {

class Handler {
  public:
    Handler() = default;

    /**
     * Input: request, Output: response.
     */
    const std::string& operator()(const std::string& req) const;

  private:
    mutable serialized::Result results_buffer;
    mutable std::vector<serialized::Sample> samples_buffer;
    mutable std::string str_buffer;

    /**
     * Populate the buffers with the request results.
     */
    serialized::Result& process(const char* buf, size_t sz) const;
};
} // namespace Request

#endif // NL_HANDLER_H_
