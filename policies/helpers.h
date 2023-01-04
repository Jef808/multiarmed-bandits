#ifndef POLICY_HELPERS_H_
#define POLICY_HELPERS_H_

#include "extactions.h"

/**
 * If the action has not been visited, return the prior
 * value, otherwise the average reward it yielded to date.
 */
inline double SampleAverage(const policy::ExtAction& e_action) {
    if (e_action.visits == 0) {
        return e_action.total;
    }
    return e_action.total / e_action.visits;
}

#endif // POLICY_HELPERS_H_
