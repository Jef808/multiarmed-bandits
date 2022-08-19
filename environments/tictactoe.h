#ifndef TICTACTOE_H_
#define TICTACTOE_H_

#include <array>
#include <iosfwd>
#include <vector>

enum class Token { E, X, O };
enum class Player { X, O };

struct Action {
    int idx;
};

class TicTacToe {
public:
    enum class Status { Ongoing, Draw, XWins, OWins };

    TicTacToe();
    bool apply_action(const Action&);
    Action apply_random_action();
    Player player_to_move() const;
    void valid_actions(std::vector<Action>& buf) const;

    Status status() const { return m_status; }
    Token operator[](int idx) const { return m_board[idx]; }
    Token& operator[](int idx) { return m_board[idx]; }

private:
    std::array<Token, 9> m_board;
    Status m_status{ Status::Ongoing };
    int n_empty_square{ 9 };
};

extern std::ostream& operator<<(std::ostream& _out, const TicTacToe& ttt);

#endif // TICTACTOE_H_
