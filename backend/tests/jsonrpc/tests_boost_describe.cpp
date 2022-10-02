#define CATCH_CONFIG_MAIN

#include "./json_call.hpp"
#include "catch2/catch.hpp"
#include "zmq_addon.hpp"

using namespace rpc;

struct Object {
  std::string greet(std::string const &who) { return "Hello, " + who + "!"; }

  int add(int x, int y) { return x + y; }
};

BOOST_DESCRIBE_STRUCT(Object, (), (greet, add))

#include <iostream>

TEST_CASE("Simple methods can be called from json object") {
  Object obj;
  boost::json::value _result1 = call(obj, "greet", {"world"});
  boost::json::value _expected1 = "Hello, world!";

  std::string_view result1 = _result1.as_string();
  std::string expected1 = "Hello, world!";

  boost::json::value _result2 = call(obj, "add", {1, 2});
  boost::json::value _expected2 = 3;

  int64_t result2 = _result2.as_int64();
  int64_t expected2 = 3;

  REQUIRE(_result1 == _expected1);
  REQUIRE(result1 == expected1);

  REQUIRE(_result2 == _expected2);
  REQUIRE(result2 == expected2);
}

TEST_CASE("Can convert multipart zmq messages into json objects") {
  zmq::multipart_t msgs;
}
