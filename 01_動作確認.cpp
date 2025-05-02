#include <bits/stdc++.h>
using namespace std;
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define all(x) (x).begin(), (x).end()
using ll = long long;

const ll MOD = 50515093L;

const int UP = 0, DOWN = 1, RIGHT = 2, LEFT = 3;
string action2char = "UDRL";

class State {
 public:
  ll seed, score;
  array<array<int, 4>, 4> board;

  State() {}

  void build() {
    cin >> seed >> score;
    cerr << "seed: " << seed << ", score: " << score << endl;

    cerr << ((seed & 0x10) == 0 ? 2 : 4) << endl;
    cerr << seed * seed % MOD << endl;

    rep(i, 4) rep(j, 4) { cin >> board[i][j]; }
  }

  vector<int> get_legal_actions() {
    vector<int> can_move(4);
    rep(i, 4) rep(j, 4) {
      if (board[i][j] != 0) {
        for (int y = i + 1; y < 4; y++) {
          if (board[y][j] == 0) {
            can_move[DOWN] = 1;
          }
        }
        for (int y = i - 1; y >= 0; y--) {
          if (board[y][j] == 0) {
            can_move[UP] = 1;
          }
        }
        for (int x = j + 1; x < 4; x++) {
          if (board[i][x] == 0) {
            can_move[RIGHT] = 1;
          }
        }
        for (int x = j - 1; x >= 0; x--) {
          if (board[i][x] == 0) {
            can_move[LEFT] = 1;
          }
        }
        if (i < 3 && board[i + 1][j] == board[i][j]) can_move[DOWN] = 1;
        if (i && board[i - 1][j] == board[i][j]) can_move[UP] = 1;
        if (j && board[i][j - 1] == board[i][j]) can_move[LEFT] = 1;
        if (j < 3 && board[i][j + 1] == board[i][j]) can_move[RIGHT] = 1;
      }
    }
    vector<int> ret;
    rep(i, 4) if (can_move[i]) ret.push_back(i);
    return ret;
  }

  // === debug ===

  void debug_board() {
    rep(i, 4) {
      rep(j, 4) cerr << board[i][j] << " ";
      cerr << endl;
    }
  }
};

int get_action(State &state) {
  int ret = -1;
  vector<int> legal_actions = state.get_legal_actions();
  if (legal_actions.size()) ret = legal_actions[0];
  return ret;
}

int main() {
  // game loop
  while (1) {
    State state;
    state.build();
    state.debug_board();

    int action = get_action(state);
    if (action == -1) {
      cout << action2char[0] << endl;
      break;
    }
    cout << action2char[action] << endl;
  }
}