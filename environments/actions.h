#ifndef ACTIONS_H_
#define ACTIONS_H_

#include <cstddef>
#include <string>

namespace env {

struct Action {
  std::size_t id;
  [[nodiscard]] std::string to_string() const { return std::to_string(id); }
  bool operator==(const Action& other) const { return id == other.id; }
  bool operator<(const Action& other) const {
    return id < other.id;
  }
};

}

#endif // ACTIONS_H_
