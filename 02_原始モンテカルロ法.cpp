#include <bits/stdc++.h>
using namespace std;
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define all(x) (x).begin(), (x).end()
using ll = long long;

template <typename T>
inline bool chmax(T &a, T b) {
  return ((a < b) ? (a = b, true) : (false));
}
template <typename T>
inline bool chmin(T &a, T b) {
  return ((a > b) ? (a = b, true) : (false));
}

#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

static uint32_t randXor() {
  static uint32_t x = 123456789;
  static uint32_t y = 362436069;
  static uint32_t z = 521288629;
  static uint32_t w = 88675123;
  uint32_t t;
  t = x ^ (x << 11);
  x = y;
  y = z;
  z = w;
  return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

class TimeKeeper {
 private:
  chrono::high_resolution_clock::time_point start_time_;
  int64_t time_threshold_;

 public:
  TimeKeeper(const int64_t &time_threshold)
      : start_time_(chrono::high_resolution_clock::now()),
        time_threshold_(time_threshold) {}

  bool isTimeOver() const {
    auto diff = chrono::high_resolution_clock::now() - this->start_time_;
    return chrono::duration_cast<chrono::milliseconds>(diff).count() >=
           time_threshold_;
  }

  int64_t getTime() const {
    auto diff = chrono::high_resolution_clock::now() - this->start_time_;
    return chrono::duration_cast<chrono::milliseconds>(diff).count();
  }
};

const ll MOD = 50515093L;

const int UP = 0, DOWN = 1, RIGHT = 2, LEFT = 3;
string action2char = "UDRL";

class State {
 public:
  ll seed, score;
  // ジャッジに合わせ、board[x][y]かつ、x右向き、y下向き
  array<array<int, 4>, 4> board;

  State() {}

  void build() {
    cin >> seed >> score;
    cerr << "seed: " << seed << ", score: " << score << endl;

    rep(y, 4) rep(x, 4) { cin >> board[x][y]; }
  }

  vector<int> get_legal_actions() {
    vector<int> can_move(4);
    rep(y, 4) rep(x, 4) {
      if (board[x][y] != 0) {
        for (int y2 = y + 1; y2 < 4; y2++) {
          if (board[x][y2] == 0) {
            can_move[DOWN] = 1;
          }
        }
        for (int y2 = y - 1; y2 >= 0; y2--) {
          if (board[x][y2] == 0) {
            can_move[UP] = 1;
          }
        }
        for (int x2 = x + 1; x2 < 4; x2++) {
          if (board[x2][y] == 0) {
            can_move[RIGHT] = 1;
          }
        }
        for (int x2 = x - 1; x2 >= 0; x2--) {
          if (board[x2][y] == 0) {
            can_move[LEFT] = 1;
          }
        }
        if (y < 3 && board[x][y + 1] == board[x][y]) can_move[DOWN] = 1;
        if (y && board[x][y - 1] == board[x][y]) can_move[UP] = 1;
        if (x && board[x - 1][y] == board[x][y]) can_move[LEFT] = 1;
        if (x < 3 && board[x + 1][y] == board[x][y]) can_move[RIGHT] = 1;
      }
    }
    vector<int> ret;
    rep(i, 4) if (can_move[i]) ret.push_back(i);
    return ret;
  }

  void advance(int action) {
    exec_action(action);
    // ボードに新しい数字を追加
    vector<int> free_cells;
    rep(x, 4) rep(y, 4) {
      if (board[x][y] == 0) {
        free_cells.push_back(x + 4 * y);
      }
    }
    if (free_cells.size()) {
      int spawn_index = free_cells[seed % (int)free_cells.size()];
      int value = (seed & 0x10) == 0 ? 2 : 4;
      board[spawn_index % 4][spawn_index / 4] = value;
    }
    // シードの更新
    seed = seed * seed % MOD;
  }

  // === debug ===

  void debug_board() {
    cerr << "seed: " << seed << ", score: " << score << endl;
    rep(y, 4) {
      rep(x, 4) cerr << board[x][y] << " ";
      cerr << endl;
    }
  }

 private:
  // 方向: UP=0, DOWN=1, RIGHT=2, LEFT=3
  void exec_action(int action) {
    vector merged(4, vector(4, 0));
    switch (action) {
      case UP: {
        rep(x, 4) {
          for (int y = 1; y < 4; y++) {
            if (board[x][y] == 0) continue;
            int next_y = -1;
            for (int y2 = y - 1; y2 >= 0; y2--) {
              if (board[x][y2] == 0) {
                // 詰めることができる
                next_y = y2;
              } else if (board[x][y2] == board[x][y] && !merged[x][y2]) {
                // mergeして終了
                board[x][y2] *= 2;
                score += board[x][y2];
                merged[x][y2] = 1;
                next_y = -1;
                board[x][y] = 0;
              } else {
                // それ以外は終わり
                break;
              }
            }
            if (next_y != -1) {
              board[x][next_y] = board[x][y];
              board[x][y] = 0;
            }
          }
        }
        break;
      }
      case DOWN: {
        rep(x, 4) {
          for (int y = 2; y >= 0; y--) {
            if (board[x][y] == 0) continue;
            int next_y = -1;
            for (int y2 = y + 1; y2 < 4; y2++) {
              if (board[x][y2] == 0) {
                // 詰めることができる
                next_y = y2;
              } else if (board[x][y2] == board[x][y] && !merged[x][y2]) {
                // mergeして終了
                board[x][y2] *= 2;
                score += board[x][y2];
                merged[x][y2] = 1;
                next_y = -1;
                board[x][y] = 0;
              } else {
                // それ以外は終わり
                break;
              }
            }
            if (next_y != -1) {
              board[x][next_y] = board[x][y];
              board[x][y] = 0;
            }
          }
        }
        break;
      }
      case LEFT: {
        rep(y, 4) {
          for (int x = 1; x < 4; x++) {
            if (board[x][y] == 0) continue;
            int next_x = -1;
            for (int x2 = x - 1; x2 >= 0; x2--) {
              if (board[x2][y] == 0) {
                // 詰めることができる
                next_x = x2;
              } else if (board[x2][y] == board[x][y] && !merged[x2][y]) {
                board[x2][y] *= 2;
                score += board[y][x2];
                merged[x2][y] = 1;
                next_x = -1;
                board[x][y] = 0;
              } else {
                break;
              }
            }
            if (next_x != -1) {
              board[next_x][y] = board[x][y];
              board[x][y] = 0;
            }
          }
        }
        break;
      }
      case RIGHT: {
        rep(y, 4) {
          for (int x = 2; x >= 0; x--) {
            if (board[x][y] == 0) continue;
            int next_x = -1;
            for (int x2 = x + 1; x2 < 4; x2++) {
              if (board[x2][y] == 0) {
                // 詰めることができる
                next_x = x2;
              } else if (board[x2][y] == board[x][y] && !merged[x2][y]) {
                board[x2][y] *= 2;
                score += board[y][x2];
                merged[x2][y] = 1;
                next_x = -1;
                board[x][y] = 0;
              } else {
                break;
              }
            }
            if (next_x != -1) {
              board[next_x][y] = board[x][y];
              board[x][y] = 0;
            }
          }
        }
        break;
      }
      default:
        assert(false);
        break;
    }
  }
};

ll playout(State &state) {
  vector<int> legal_actions = state.get_legal_actions();
  if (legal_actions.empty()) {
    return state.score;
  }
  int action = legal_actions[randXor() % (int)legal_actions.size()];
  state.advance(action);
  return playout(state);
}

int decide_action_montecarlo(State &state, TimeKeeper &tk) {
  ll best_score = 0;
  int best_action = -1;
  vector<int> legal_actions = state.get_legal_actions();
  int loop_count = 0;

  while (!tk.isTimeOver()) {
    for (int action : legal_actions) {
      State new_state = state;
      new_state.advance(action);
      ll score = playout(new_state);
      loop_count++;
      if (chmax(best_score, score)) best_action = action;
    }
  }
  cerr << "loop_count: " << loop_count << endl;
  return best_action;
}

void test() {
  {
    // exex_actionのテスト
    /*
    4 4 32 8
    16 16 2 16
    8 8 8 4
    2 2 2 2
    */
    State state;
    state.score = 260;
    state.seed = 45490976;
    state.board[2][1] = state.board[0][3] = state.board[1][3] =
        state.board[2][3] = state.board[3][3] = 2;
    state.board[0][0] = state.board[1][0] = state.board[3][2] = 4;
    state.board[3][0] = state.board[0][2] = state.board[1][2] =
        state.board[2][2] = 8;
    state.board[0][1] = state.board[1][1] = state.board[3][1] = 16;
    state.board[2][0] = 32;
    state.debug_board();
    //
    state.advance(RIGHT);
    /*
    0 8 32 8
    2 32 2 16
    0 8 16 4
    0 0 4 4
    */
    assert(state.score = 324);
    state.debug_board();
  }
}

void local_play() {
  State state;
  state.board[1][3] = 4;
  state.board[3][1] = 2;
  state.score = 0;
  state.seed = 13339144;

  while (1) {
    TimeKeeper tk(40);
    int action = decide_action_montecarlo(state, tk);
    if (action == -1) {
      break;
    }
    state.advance(action);
    state.debug_board();
    cerr << action2char[action] << endl;
  }
  state.debug_board();
}

int main() {
  // test();
  local_play();
  return 0;

  // game loop
  while (1) {
    State state;
    state.build();
    state.debug_board();

    TimeKeeper tk(45);
    int action = decide_action_montecarlo(state, tk);
    if (action == -1) {
      cout << action2char[0] << endl;
      break;
    }
    cout << action2char[action] << endl;
  }
}