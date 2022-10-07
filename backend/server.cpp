#include "server.hpp"
#include "execute_request.hpp"
#include "process_request.hpp"
#include "zhelpers.hpp"

#include <boost/json.hpp>
#include <boost/json/src.hpp>

#include <algorithm>
#include <cassert>
#include <condition_variable>
#include <mutex>
#include <string>
#include <utility>
#include <variant>

zmq::context_t context_g{};

namespace json = boost::json;

server_t::server_t(const std::string &addr)
    : socket_{std::make_shared<zmq::socket_t>(context_g, ZMQ_REP)}, job_{} {
  assert(!addr.empty());
  socket_->bind(addr);
  init();
  std::cout << "server: listening to " << addr << std::endl;
}

server_t::server_t(server_t &&server_) noexcept
    : request_waiting_{server_.request_waiting_},
      socket_{std::move(server_.socket_)}, job_{} {
  init();
}

server_t &server_t::operator=(server_t &&server_) noexcept {
  {
    std::unique_lock<std::mutex> lock(server_.m_);
    {
      if (server_.socket_ != socket_) {
        std::unique_lock<std::mutex> lock_(m_);

        socket_ = std::exchange(server_.socket_, {});
        request_waiting_ = std::exchange(server_.request_waiting_, false);
        init();
      }
    }
  }
  return *this;
}

/**
 * Use the condition_variable to notify thread of changes to this->running_
 */
void server_t::init() noexcept {
  (job_ = std::thread([this] {
     for (;;) {
       std::unique_lock<std::mutex> lock(m_);
       cv_.wait(lock, [this] { return request_waiting_; });

       std::string req = s_recv(*socket_);
       json::object rep_obj = handle_request(req);
       std::string rep = json::serialize(rep_obj);
       s_send(*socket_, rep);

       request_waiting_ = false;
     }
   })).detach();
}

void server_t::set_active() noexcept {
  {
    std::unique_lock<std::mutex> lock(m_);
    request_waiting_ = true;
  }
  cv_.notify_one();
}

/**
 * Pause the job
 */
void server_t::set_idle() noexcept {
  {
    std::unique_lock<std::mutex> lock(m_);
    request_waiting_ = false;
  }
  cv_.notify_one();
}

std::string server_t::process_request(const std::string &req) noexcept {
  json::object req_j = parse_request(req);
}

std::ostream &operator<<(std::ostream &os, const server_t &server) {
  return os << "server: Socket = " << server.socket_ << std::endl;
}
