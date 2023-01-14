#include "tictactoe.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Display methods
////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& _out, Token tok) {
    return _out << ' '
                << (tok == Token::X ? " X ":
                    tok == Token::O ? " O " : "   ");
}

std::ostream& operator<<(std::ostream& _out, const TicTacToe& ttt) {
    for (int row = 0; row < 3; ++row) {
        _out << '|';
        for (int col = 0; col < 3; ++col) {
            auto idx = col + 3 * row;
            _out << ttt[idx]
                 << (col < 2 ? "|" : "|\n");
        }

    }
    return _out;
}

namespace {

std::array<std::array<int, 3>, 8> Lines {
std::array<int, 3>{0,1,2},
{3,4,5},
{6,7,8},
{0,3,6},
{1,4,7},
{2,5,8},
{0,4,8},
{2,4,6}
};

Token token_of(Player p) {
    return p==Player::X? Token::X: Token::O;
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// Game mechanics
////////////////////////////////////////////////////////////////////////////////
TicTacToe::TicTacToe()
    : m_status{ Status::Ongoing }
    , m_board{}
{
    m_board.fill(Token::E);
}

bool TicTacToe::apply_action(const Action & action) {
    Token& sq = m_board[action.idx];
    if (sq != Token::E) {
        std::cerr << "Tried to play at " << action.idx << std::endl;
        throw std::runtime_error("Invalid action");
    }

    sq = token_of(player_to_move());
    --n_empty_square;

    for (const auto& line : Lines) {
        bool winning_move = std::all_of(line.begin(), line.end(), [&](auto i){
            return m_board[i] == sq;
        });
        if (winning_move) {
            m_status = sq==Token::X? Status::XWins: Status::OWins;
            return false;
        }
    }
    if (m_status == Status::Ongoing && (n_empty_square) == 0) {
        m_status = Status::Draw;
    }

    return m_status == Status::Ongoing;
}

Player TicTacToe::player_to_move() const {
    return Player(!(n_empty_square & 1));
}

void TicTacToe::valid_actions(std::vector<Action>& buf) const {
    if (m_status != Status::Ongoing) {
        return;
    }
    for (int i = 0; i < m_board.size(); ++i) {
        if (m_board[i] == Token::E) {
            buf.push_back({i});
        }
    }
}
