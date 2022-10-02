#include "myzmqhelpers.hpp"
#include <zhelpers.hpp>

const std::string frontend_addr = "tcp://*:5559";
const std::string backend_addr = "tcp://*:5560";

int main(int argc, char *argv[]) {
  zmq::context_t context(1);

  // Socket facing clients
  zmq::socket_t frontend(context, ZMQ_ROUTER);
  frontend.bind(frontend_addr);

  // Socket facing services
  zmq::socket_t backend(context, ZMQ_DEALER);
  backend.bind(backend_addr);

  // Capture socket
  // NOTE: socket_ref is nullable
  zmq::socket_ref capture = zmq::socket_ref();

  // Start the proxy
  zmq::proxy(frontend, backend, capture);

  return 0;
}
