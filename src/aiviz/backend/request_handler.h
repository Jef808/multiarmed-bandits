#ifndef REQUEST_HANDLER_H_
#define REQUEST_HANDLER_H_

#include <string>

namespace Query {

class RequestHandler {
  public:
    RequestHandler() = default;

    std::pair<bool, std::string> operator()(std::string&& request);
};

} // namespace Query

#endif // REQUEST_HANDLER_H_
