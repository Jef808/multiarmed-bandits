//
//  Hello World server in C++
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include "helpers.hpp"
#include <iostream>
#include <string>

#include <zmq.hpp>

int main() {
  //  Prepare our context and socket
  zmq::context_t context(2);
  zmq::socket_t socket(context, zmq::socket_type::rep);
  socket.bind("tcp://*:5555");

  while (true) {
    zmq::message_t request;

    //  Wait for next request from client
    auto bytes_transferred = socket.recv(request, zmq::recv_flags::none);
    std::cout << "server:received request=" << request.to_string_view()
              << std::endl;

    //  Do some 'work'
    sleep_sync<timescale::s>(1);

    //  Send reply back to client
    zmq::message_t reply(5);
    memcpy(reply.data(), "World", 5);
    socket.send(reply, zmq::send_flags::none);
  }
  return 0;
}
