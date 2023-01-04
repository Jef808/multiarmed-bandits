#ifndef REQUEST_HANDLER_H_
#define REQUEST_HANDLER_H_

#include <boost/json.hpp>
#include "boost_serializer_helper.h"

#include <string>
#include <vector>

namespace QuerySerialize {

class RequestHandler {
    public:
        RequestHandler() = default;

        std::pair<bool, std::string> operator()(const std::string& request);

    private:
        Series series_buffer;
};

}

#endif // REQUEST_HANDLER_H_
