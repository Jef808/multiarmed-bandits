#ifndef QUEUE_H_
#define QUEUE_H_

#include <deque>
#include <memory>

template <typename T = std::string>
class MessageQueue {
  public:
    MessageQueue() = default;

    /**
     * Push at the back of the queue
     */
    void push(T&& msg) {
      m_data.emplace_back(msg);
    }

    /**
     * Get the first item in the queue (removing it from queue)
     */
    std::unique_ptr<T> pop() {
      auto msg = std::move(m_data.front());
      m_data.pop_front();
      return msg;
    }

  private:
    std::deque<std::unique_ptr<T>> m_data;
};


#endif // QUEUE_H_
