//
//  Hello World client in C++
//  Connects REQ socket to tcp://localhost:5555
//  Sends "Hello" to server, expects "World" back
//
#include "helpers.hpp"
#include <iostream>
#include <string>
#include <zhelpers.hpp>

const zmq::message_t EMPTY_MSG(0);

int main() {
  //  Prepare our context and socket
  zmq::context_t context(1);
  zmq::socket_t dealer(context, ZMQ_DEALER);

  std::cout << "Connecting to hello world server..." << std::endl;
  zmq_connect(dealer, "tcp://localhost:5555");

  //  Do 10 requests, waiting each time for a response
  for (int request_nbr = 0; request_nbr != 10; request_nbr++) {
    const std::string s_request = "Hello";
    std::cout << "client:sending request=" << s_request << " #" << request_nbr
              << std::endl;

    s_sendmore(dealer, std::string());
    s_send(dealer, s_request);

    //  Get the reply.
    std::string reply = s_recv(dealer);
    bool finished = reply.empty();

    if (finished) {
      std::cout << "client:discarding invalid response=" << reply << '\n';
    }

    while (!finished) {
      reply += s_recv(dealer);
      finished = reply == "World";
    }

    // auto bytes_transferred = socket.recv(reply, zmq::recv_flags::none);
    std::cout << "client:received response=" << reply << '\n';
  }
  return 0;
}
