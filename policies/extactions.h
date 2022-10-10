#ifndef EXTACTIONS_H_
#define EXTACTIONS_H_

#include <cstddef>

/**
 * Structure wrapping actions with additional data.
 */
template <typename Action> struct ExtAction {
    explicit ExtAction(size_t idx)
        : action{idx} { }

    // The data to attach to actions during a search.
    Action action;
    int visits{0};
    double total{0.0};

    operator Action() const { return action; }

    /**
     * In order e.g. to use `std::find()` on ExtActions
     */
    bool operator==(const Action& a) const { return this->action == action; }
};

#endif // EXTACTIONS_H_
