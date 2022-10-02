#define CATCH_CONFIG_MAIN

#include "json_call.hpp"
#include "json_recv.hpp"
#include "json_validate.hpp"
#include "tests/zhelpers.hpp"
#include <boost/json.hpp>
#include <zmq_addon.hpp>

#include <catch2/catch.hpp>

namespace json = boost::json;
using namespace rpc;

class JsonMsgTestsFixture {
public:
  json::object obj;
  json::value jv;
  std::string s_obj =
      R"({{"command","CommandName"},{"args",{{"opt1",10},{"opt2",1.4},{"opt3",true}}},{"series",{{1,2E-1},{2,6E-1},{3,-2E-1},{4,5E-1},{5,1E-1}}}})";

  JsonMsgTestsFixture()
      : obj{{{"command", "CommandName"},
             {"args", {{"opt1", 10}, {"opt2", 1.4}, {"opt3", true}}},
             {"series", {{1, 0.2}, {2, 0.6}, {3, -0.2}, {4, 0.5}, {5, 0.1}}}}},
        jv{{json::value_ref(obj)}}, s_obj{json::serialize(obj)} {

    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
  }
};

TEST_CASE_METHOD(JsonMsgTestsFixture,
                 "json::serialize() outputs the correct string") {
  REQUIRE(json::serialize(obj) == s_obj);
}

TEST_CASE_METHOD(JsonMsgTestsFixture,
                 "json::parse and json::serialize are inverse of each other "
                 "when acting on objects.") {
  REQUIRE(obj == json::parse(s_obj));
  REQUIRE(json::parse(json::serialize(obj)) == obj);
  REQUIRE(json::serialize(json::parse(s_obj).as_object()) == s_obj);
}

TEST_CASE_METHOD(JsonMsgTestsFixture, "Validate returns true on valid data") {
  REQUIRE(validate(json::serialize(obj)));
}
