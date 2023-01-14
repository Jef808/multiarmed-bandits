#ifndef EXTACTIONS_H_
#define EXTACTIONS_H_

#include "aiviz/environments/actions.h"

using Action = env::Action;

namespace policy {

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
