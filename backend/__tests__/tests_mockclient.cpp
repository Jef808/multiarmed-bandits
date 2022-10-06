#define CONFIG_CATCH_MAIN

#include "../data.hpp"
#include "../server.hpp"
#include "../zhelpers.hpp"
#include <boost/json.hpp>
#include <catch2/catch_all.hpp>
//#include <catch2/catch.hpp>

namespace json = boost::json;

TEST_CASE("Mock interaction with server works as expected") {

  zmq::socket_t client{context_g, ZMQ_REQ};
  client.connect("tcp://localhost:5555");
  server_t server{"tcp://*:5555"};
  server.start();

  // json::object ro{};
  std::string req1 = json::serialize(json::object({{"test", "dummy"}}));
  std::string req2 = json::serialize(json::object({{"test", "data"}}));
  std::string req3 = json::serialize(json::object({{"model", "mab"}}));
  std::string rep1;
  std::string rep2;
  std::string rep3;
  std::string exp1 = json::serialize(
      json::object({{"status", "success"}, {"message", "Okay"}}));
  std::string exp2 = json::serialize(test_data_1);
  std::string exp3 = json::serialize(test_data_2);

  s_send(client, req1);
  rep1 = s_recv(client);

  s_send(client, req2);
  rep2 = s_recv(client);

  s_send(client, req3);
  rep3 = s_recv(client);

  INFO("Shutting down server");
  server.stop();
  INFO("Closing client socket");
  client.close();

  REQUIRE(rep1 == exp1);
  REQUIRE(rep2 == exp2);
  REQUIRE(rep3 == exp3);

  // REQUIRE(json::parse(rep) ==);

  // ro = {{"test", "data"}};
  // req = json::serialize(ro);
  // s_send(client, req);

  // rep = s_recv(client);

  // INFO("Data test request");

  // REQUIRE(json::parse(rep) == test_data_1);

  // ro = {{"model", "mab"}};
  // req = json::serialize(ro);
  // s_send(client, req);

  // rep = s_recv(client);

  // INFO("Valid model request");

  // REQUIRE(json::parse(rep) == test_data_2);

  // server.stop();
  // client.close();
}

// TEST_CASE("Test data request is replied by prescribed data response") {

//   server_t server{"tcp://*:5555"};
//   zmq::socket_t client{context_g, ZMQ_REQ};
//   client.connect("tcp://localhost:5555");

//   server.start();

//   json::object ro{{"test", "data"}};
//   std::string req = json::serialize(ro);
//   s_send(client, req);

//   std::string rep;
//   REQUIRE_NOTHROW(rep = s_recv(client));

//   server.stop();

//   REQUIRE(json::parse(rep) == test_data_1);
// }

// TEST_CASE("Request with valid model replied by mocked response") {
//   server_t server{"tcp://*:5555"};
//   zmq::socket_t client{context_g, ZMQ_REQ};
//   client.connect("tcp://localhost:5555");
//   server.start();

//   json::object ro{{"model", "mab"}};
//   std::string req = json::serialize(ro);
//   s_send(client, req);

//   std::string rep;
//   REQUIRE_NOTHROW(rep = s_recv(client));

//   server.stop();

//   REQUIRE(json::parse(rep) == test_data_2);
// }
