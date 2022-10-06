#include "server.hpp"
#include "data.hpp"
#include "zhelpers.hpp"

#include <boost/json.hpp>
#include <boost/json/src.hpp>

#include <algorithm>
#include <cassert>
#include <condition_variable>
#include <mutex>
#include <string>
#include <utility>

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
       json::object rep_obj = process_request(req);
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

json::object process_valid_request(const json::object &jo) noexcept {
  return jo;
}

json::object server_t::process_request(const std::string &req) noexcept {
  using std::cerr;
  using std::endl;

  static const json::object empty_jo{};
  json::object jo = empty_jo;

  if (!req.empty()) {

    json::value jv = json::parse(req);

    if (jv.is_object()) {

      jo = jv.as_object();

      static std::vector<std::string> test_names = {"dummy", "data"};

      const bool is_test = [&jo] {
        try {
          string_view test_name = jo["test"].as_string();
          return !test_name.empty();
        } catch (std::exception &e) {
          return false;
        }
      }();

      // If is test request
      if (is_test) {

        string_view test_name = jo["test"].as_string();

        if (test_name == "dummy") {

          jo = json::object({{"status", "success"}, {"message", "Okay"}});

        } else if (test_name == "data") {

          jo = json::object(test_data_1);

        } else {
          std::cerr << "process_request: invalid test name: " << test_name
                    << std::endl;
          return jo;
        }
      }

      // Not test request
      else {

        // Process model
        const auto [has_model, model_name] = [&jo] {
          try {
            string_view model_name =
                jo["model"].as_object()["name"].as_string();

            return std::pair<bool, string_view>{true, model_name};

          } catch (std::exception &e) {

            return std::pair<bool, string_view>{false, ""};
          }
        }();

        if (has_model) {

          if (model_name == "mab") {

            // TODO... merge jo

          } else { // invalid model name

            std::cerr << "process_request: invalid model name: " << model_name
                      << std::endl;
          }
        } else { // doesn't have model field

          std::cerr
              << "process_request: invalid request: " << jo
              << "\nrequest must contain valid model name field of the form"
              << R"({"model": { "name": "<model_name>", }, })" << std::endl;
        }

        // Process policy
        const auto [has_policy, policy_name] = [&jo] {
          try {
            string_view policy_name =
                jo["policy"].as_object()["name"].as_string();

            return std::pair<bool, string_view>{true, policy_name};

          } catch (std::exception &e) {

            return std::pair<bool, string_view>{false, ""};
          }
        }();

        if (has_policy) {

          if (policy_name == "epsilon_greedy") {

            // TODO...

          }

          else if (policy_name == "ucb") {

            // TODO...

          }

          else { // invalid policy name

            std::cerr << "process_request: invalid policy name: " << policy_name
                      << std::endl;
          }
        } else { // doesn't have policy field

          std::cerr << "process_request: invalid request: " << jo
                    << "\nrequest must contain valid policy field of the form"
                    << R"({"policy": { "name": "<policy_name>", }, })"
                    << std::endl;
        }

      } // endif (is_test || !is_test)

    } // endif (is_object())

    else {
      cerr << "server: request is not valid json object" << endl;
    }
  } // endif (is_empty())

  else {
    cerr << "server: Warning: received empty message" << endl;
  }

  return jo;
}

std::ostream &operator<<(std::ostream &os, const server_t &server) {
  return os << "server: Socket = " << server.socket_ << std::endl;
}
