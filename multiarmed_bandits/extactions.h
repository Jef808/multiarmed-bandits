#ifndef EXTACTIONS_H_
#define EXTACTIONS_H_

#include <cstddef>

/**
 * Structure to accumulate data along the search.
 */
template <typename Action> struct ExtActionT {
  explicit ExtActionT(size_t idx) : action{idx} {}

  Action action;
  int visits{0};
  double total{0.0};

  operator Action() const { return action; }
  bool operator==(const Action &a) const;
};

template <typename Action>
bool ExtActionT<Action>::operator==(const Action &action) const {
  return this->action == action;
}

#endif // EXTACTIONS_H_
