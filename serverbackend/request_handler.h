#ifndef REQUEST_HANDLER_H_
#define REQUEST_HANDLER_H_

#include "environments/actions.h"
#include "policies/agent.h"

#include <boost/json.hpp>

#include <string>
#include <vector>

namespace Query {

class RequestHandler {
    public:
        RequestHandler() = default;

        std::pair<bool, std::string> operator()(const std::string& request);

};

}

#endif // REQUEST_HANDLER_H_
