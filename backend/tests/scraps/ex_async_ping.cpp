#include <array>
#include <atomic>
#include <azmq/actor.hpp>
#include <azmq/socket.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace asio = boost::asio;
namespace json = boost::json;

/**
 * Implement server's frontend (to which we make requests)
 * and backend (which sends reponses) as two endpoints of a pipe (pair of inproc
 * sockets).
 *
 * spawn(caller_ios, run, pstate) returns an 'Actor' attached to the
 */
class server_t {
public:
  server_t(asio::io_service &caller_ios)
      : frontend_{azmq::actor::spawn(caller_ios, run, pstate_)},
        pstate_{std::make_shared<backend_state>()} {}

  /**
   * Frontend socket ping task:
   *
   *   - sync_send ('PING'),
   *   - async_receive ({
   *       _try_until_: frontend buffer contains 'PONG' OR _error_
   *   })
   * }).
   */
  void ping() {
    frontend_.send(asio::buffer("PING"));
    frontend_.async_receive(
        asio::buffer(buf_),
        [this](boost::system::error_code const &ec, size_t bytes_transferred) {
          if (ec)
            return; // exit on error

          if (std::string_view(buf_.data(), bytes_transferred - 1) == "PONG") {
            pstate_->pongs_++;
          }
        });
  }

  friend std::ostream &operator<<(std::ostream &stm, server_t const &that) {
    return stm << "pings=" << that.pstate_->pings_
               << ", pongs=" << that.pstate_->pongs_;
  }

private:
  struct backend_state {
    std::array<char, 256> buf_;
    std::atomic<unsigned long> pings_{0};
    std::atomic<unsigned long> pongs_{0};
  };

  using ptr = std::shared_ptr<backend_state>;
  ptr pstate_;

  // Backend socket:
  //
  // if state mutex is available...
  //
  // - async_receive({
  //   _try_while_: state mutex is available, _NO_ERROR_, state's buffer
  //
  // })
  static void do_receive(azmq::socket &backend,
                         std::weak_ptr<backend_state> pstate) {
    if (auto p = pstate.lock()) {
      backend.async_receive(
          asio::buffer(p->buf_),
          [&backend, pstate](boost::system::error_code const &ec,
                             size_t bytes_transferred) {
            if (ec)
              return; // exit on error

            if (auto p = pstate.lock()) {
              if (std::string_view(p->buf_.data(), bytes_transferred - 1) !=
                  "PING")
                return; // exit if not PING

              p->pings_++;
              backend.send(asio::buffer("PONG"));

              // schedule another receive
              do_receive(backend, pstate);
            }
          });
    }
  }

  // this is the function run by the background thread
  static void run(azmq::socket &backend, ptr pstate) {
    do_receive(backend, pstate);
    backend.get_io_service().run();
  }

  azmq::socket frontend_;
  std::array<char, 256> buf_;
};

/**
 * Schedule server PING every 250ms while _NO_ERROR_.
 */
void schedule_ping(asio::steady_timer &timer, server_t &server) {
  server.ping();

  timer.expires_after(asio::chrono::milliseconds(250));
  timer.async_wait([&](std::error_code const &ec) {
    if (ec)
      return;
    schedule_ping(timer, server);
  });
}

/**
 * Starting io_service is the ''
 */
int main(int argc, char *argv[]) {
  asio::io_service ios;

  std::cout << "Running async ping/pong...\n";
  std::cout.flush();

  // halt on SIGINT or SIGTERM
  asio::signal_set signals(ios, SIGTERM, SIGINT);
  signals.async_wait(
      [&](boost::system::error_code const &, int) { ios.stop(); });

  server_t server(ios);

  asio::steady_timer timer(ios);
  schedule_ping(timer, server);

  // run for 5 seconds
  asio::steady_timer deadline(ios, asio::chrono::seconds(5));
  deadline.async_wait([&](boost::system::error_code const &) { ios.stop(); });

  ios.run();

  std::cout << "Done. Results - " << server << std::endl;

  return EXIT_SUCCESS;
}
