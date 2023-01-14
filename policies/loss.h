#include <algorithm>
#include "environments/actions.h"
#include "environments/multiarmed_bandits/multiarmed_bandits.h"
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace policy {

    template<typename Environment>
    struct Loss {
      static constexpr double THREE_STD_DEV = 3.0;

      Loss(const Environment& env)
      {
        if constexpr (std::is_same_v<Environment, env::MultiArmedBandit>) {
          assert(env.number_of_actions() > 1);  // NOLINT
          std::generate_n(std::back_inserter(expectations), env.number_of_actions(), [&env, n=0UL]() mutable {
            return env.expectation(env::Action{n++});
          });
          max_expectation = *std::max_element(expectations.begin(), expectations.end());
          min_expectation = *std::min_element(expectations.begin(), expectations.end());
          std::cerr << "max_expectation: " << max_expectation << std::endl;
          std::cerr << "min_expectation: " << min_expectation << std::endl;
        }
      }

      double operator()(const env::Action& action, double reward) const {
        if constexpr (std::is_same_v<Environment, env::MultiArmedBandit>) {
          return (max_expectation - expectations[action.id]) / (max_expectation - min_expectation);
        }

        throw std::runtime_error("Unimplemented environment");
      }

      std::vector<double> expectations;
      double max_expectation;
      double min_expectation;
    };
};
