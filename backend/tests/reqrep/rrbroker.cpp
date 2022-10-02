#include <tests/zhelpers.hpp>

const std::string frontend_addr = "tcp://*:5559";
const std::string backend_addr = "tcp://*:5560";

int main(int argc, char *argv[]) {

  //  Prepare our context and socket
  zmq::context_t context(1);
  zmq::socket_t frontend(context, ZMQ_ROUTER);
  zmq::socket_t backend(context, ZMQ_DEALER);

  frontend.bind(frontend_addr);
  backend.bind(backend_addr);

  // Initialize poll set
  zmq::pollitem_t items[] = {{frontend, 0, ZMQ_POLLIN, 0},
                             {backend, 0, ZMQ_POLLIN, 0}};

  const unsigned int nb_of_items = 2;
  auto constexpr FOREVER =
      std::numeric_limits<std::chrono::milliseconds>::infinity();

  // Switch messages between sockets
  for (;;) {
    zmq::message_t message;
    int more;

    zmq::poll(&items[0], nb_of_items, FOREVER);

    // Poll frontend
    if (items[0].revents & ZMQ_POLLIN)
      while (true) {
        auto rc = frontend.recv(message, zmq::recv_flags::none);
        size_t more_size = sizeof(more);
        more = frontend.get(zmq::sockopt::rcvmore);
        // Manually send empty first frame.
        backend.send(message,
                     more ? zmq::send_flags::sndmore : zmq::send_flags::none);
        if (!more)
          break;
      }
    // Poll backend
    if (items[1].revents & ZMQ_POLLIN) {
      while (true) {
        auto rc = backend.recv(message, zmq::recv_flags::none);
        size_t more_size = sizeof(more);
        more = backend.get(zmq::sockopt::rcvmore);
        // Manually send empty first frame.
        frontend.send(message,
                      more ? zmq::send_flags::sndmore : zmq::send_flags::none);
        if (!more)
          break;
      }
    }
  }
}
