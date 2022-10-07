#ifndef SERVER_H_
#define SERVER_H_

#include <condition_variable>
#include <iosfwd>
#include <memory>
#include <mutex>
#include <thread>
#include <zmq.hpp>

extern zmq::context_t context_g;

class server_t {
  public:
    server_t() = default;
    explicit server_t(const std::string& addr);

    server_t(server_t&& server_) noexcept;
    server_t& operator=(server_t&& server_) noexcept;

    server_t(const server_t& server_) = delete;
    server_t& operator=(const server_t& server_) = delete;

    ~server_t() = default;

    void init() noexcept;
    void notify() noexcept;
    void pause() noexcept;

  private:
    bool request_waiting_{false};
    std::shared_ptr<zmq::socket_t> socket_;
    std::thread job_;
    std::condition_variable cv_;
    std::mutex m_;
    std::mutex m_end_;

    std::string process_request(const std::string& req) noexcept;

    friend std::ostream& operator<<(std::ostream& os, const server_t& server);
};

extern std::ostream& operator<<(std::ostream& os, const server_t& server);

#endif // SERVER_H_
