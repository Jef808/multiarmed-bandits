#ifndef EXTACTIONS_H_
#define EXTACTIONS_H_

#include <cstddef>
#include <string>
#include <type_traits>


namespace policy {

#include "policies/extactions.h"

namespace policy {

bool operator<(const Action& a, const Action& b) {
  return a.id < b.id;
}


/**
 * Structure wrapping actions with additional data.
 */
struct ExtAction {
    explicit ExtAction(Action action)
        : action{action} { }

    ExtAction(Action action, double reward, int visits=0)
        : action{action}, total{reward}, visits{visits} {}

    operator Action() const { return action; }

    // The data to attach to actions during a search.
    Action action;
    double total{0.0};
    int visits{0};
};

}  // namespace policy

#endif // EXTACTIONS_H_
