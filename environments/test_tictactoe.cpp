#include "environments/tictactoe.h"

#include <cstddef>
#include <iostream>
#include <random>

int main() {

    std::random_device rd;
    std::mt19937 gen{rd()};

    TicTacToe ttt{};

    std::vector< Action > buf;
    ttt.valid_actions(buf);

    bool okay = not buf.empty();
    std::cout << "Beginning main loop" << std::endl;

    while (okay) {
        std::uniform_int_distribution<> dist(0, buf.size() - 1);
        int idx = dist(gen);
        Action action = buf[idx];
        okay = ttt.apply_action(action);

        std::cout << "Action: " << action.idx << std::endl;
        std::cout << ttt << std::endl;

        buf.clear();
        ttt.valid_actions(buf);
        okay &= not buf.empty();
    }

    std::cout << "\nDone!" << std::endl;
    Token winner =
        ttt.status() == TicTacToe::Status::XWins? Token::X:
        ttt.status() == TicTacToe::Status::OWins? Token::O: Token::E;
    std::cout << (winner == Token::X? "X wins!": winner == Token::O? "O wins!": "Game is drawn!")
        << std::endl;

    return EXIT_SUCCESS;
}
