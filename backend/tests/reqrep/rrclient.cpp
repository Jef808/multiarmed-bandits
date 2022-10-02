//
//  Hello World client in C++
//  Connects REQ socket to tcp://localhost:5555
//  Sends "Hello" to server, expects "World" back
//
#include <string>
#include <tests/zhelpers.hpp>

const std::string frontend_addr = "tcp://localhost:5559";

int main(int argc, char *argv[]) {
  //  Prepare our context and socket
  zmq::context_t context(1);
  zmq::socket_t requester(context, ZMQ_REQ);
  requester.connect(frontend_addr);

  std::cout << "client:connected to " << frontend_addr << std::endl;

  // zmq::socket_t dealer(context, ZMQ_DEALER);

  const std::string req = "Hello";
  //  Do 10 requests, waiting each time for a response
  for (int req_nb = 0; req_nb != 10; ++req_nb) {
    std::cout << "client:sending request= [" << req << " ] ... #" << req_nb
              << '\n';
    s_send(requester, req);

    std::string rep = s_recv(requester);
    std::cout << "client:received response= [" << rep << " ] ... #" << req_nb
              << '\n';
  }
  return 0;
}
