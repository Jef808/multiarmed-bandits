#ifndef NL_HANDLER_H_
#define NL_HANDLER_H_

#include <string>
#include <vector>

#include "serialized.h"

class request_handler_t {
  public:
    request_handler_t() = default;

    /**
     * Input: request, Output: response.
     */
    std::string operator()(const std::string& request);

  private:
    std::vector<serialized::sample> results_buffer;
};

#endif // NL_HANDLER_H_
