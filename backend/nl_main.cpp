#include "include/zhelpers.hpp"
#include "server.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace nlohmann::literals;

int main(int argc, char* argv[]) {

    server_t server{"tcp://*:5555"};
    zmq::socket_t client(context_g, ZMQ_REQ);

    server.init();
    client.connect("tcp://localhost:5555");

    json req =
        R"({"model":{"name":"mab","parameters":{"numberOfArms": 10}},"policy":{"name":"ucb","parameters":{"exploration":0.7}},"parameters":{"numberOfSteps":1000}})"_json;

    s_send(client, req.dump());
    std::cout << "Request: " << req.dump(4) << std::endl;
    server.notify();

    std::string rep = s_recv(client);
    std::cout << "Response: " << rep << std::endl;

    return 0;
}
