#ifndef DATA_BOOST_H_
#define DATA_BOOST_H_

#include <boost/json.hpp>

using namespace boost::json;

static inline const object test_data_1 = {{{"step", 1},
                                           {"action", 2},
                                           {"value", 0.2},
                                           {"step", 2},
                                           {"action", 1},
                                           {"value", 0.6},
                                           {"step", 3},
                                           {"action", 1},
                                           {"value", 0.0},
                                           {"step", 4},
                                           {"action", 2},
                                           {"value", 0.2},
                                           {"step", 5},
                                           {"action", 1},
                                           {"value", 0.5}}};

static inline const object test_data_2 = {{{"step", 1},
                                           {"action", 1},
                                           {"value", 0.4},
                                           {"step", 2},
                                           {"action", 2},
                                           {"value", 0.2},
                                           {"step", 3},
                                           {"action", 1},
                                           {"value", 0.6},
                                           {"step", 4},
                                           {"action", 2},
                                           {"value", 0.2},
                                           {"step", 5},
                                           {"action", 1},
                                           {"value", 0.5}}};

#endif // DATA_BOOST_H_
