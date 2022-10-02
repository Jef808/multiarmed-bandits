//
//  Hello World server in C++
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <tests/zhelpers.hpp>

const std::string backend_addr = "tcp://localhost:5560";

int main(int argc, char *argv[]) {
  //  Prepare our context and socket
  zmq::context_t context(1);
  zmq::socket_t responder(context, ZMQ_REP);

  try {
    responder.connect(backend_addr);
  } catch (std::exception &e) {
    std::cerr << "worker:connect: " << e.what() << std::endl;
  }

  const std::string rep = "World";

  for (;;) {
    // Wait for next request from client
    std::string request = s_recv(responder);
    std::cout << "worker:received request = [" << request << "]\n";

    // Process the request
    sleep(1);

    // Send back reply to client
    s_send(responder, rep);
  }

  return 0;
}
