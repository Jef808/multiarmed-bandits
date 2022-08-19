#include "multiarmed_bandits.h"
#include "agent_greedy.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>

auto constexpr default_n_actions = 10;


int main(int argc, char *argv[]) {
    size_t number_of_actions = default_n_actions;
    double epsilon = 0.1;

    if (argc > 1) {
        try {
            epsilon = std::stod(argv[1]);
        }
        catch(std::exception& e){
            std::cerr << "Invalid format for `epsilon` (must be a floating point number)" << std::endl;
        }
    }

    if (argc > 2) {
        try {
            number_of_actions = std::stoi(argv[2]);
        }
        catch(std::exception& e) {
            std::cerr << "Invalid format for `number_of_actions` (must be a positive integer)" << std::endl;
        }
    }

    std::cout << "\n********\n\033[1m    Number of actions\033[0m: " << number_of_actions
        << ", \033[1mepsilon\033[0m: " << epsilon << '\n' << std::endl;

    NArmedBandit bandit{ number_of_actions };

    AgentGreedy agent{bandit};

    for (size_t i = 0; i < 1000; ++i) {
        agent.sample();
    }

    const auto& results = agent.get_data();

    auto loss = [&](const auto& ea)
    {
        if (ea.visits == 0)
            return 0.0;
        return std::abs(ea.total/ea.visits - bandit.expectation(ea));
    };

    std::sort(results.begin(), results.end(), [&](const auto& a, const auto& b) {
        return loss(a) < loss(b);
    });

    size_t i = 0;
    for (const auto& ea : results) {
        std::cout << std::setprecision(2)
            << "\033[1mAction\033[0m " << i++
                  << ": Visits = " << ea.visits
            << ", Avg: ";
        if (ea.visits > 0) {
            std::cout << ea.total/ea.visits;
        }
        else {
            std::cout << "NAN";
        }
        std::cout << "real value: "
                  << bandit.expectation(ea.action)
                  << ", Loss = ";
        if (ea.visits > 0) {
            std::cout << std::abs(ea.total/ea.visits - bandit.expectation(ea.action));
        }
        else {
            std::cout << "NAN";
        }
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}
