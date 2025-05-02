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
unordered_map<char, int> char2action = {
    {'U', UP}, {'D', DOWN}, {'R', RIGHT}, {'L', LEFT}};

class State {
 public:
  ll seed;
  // ゲームの生スコア
  ll score;
  // ジャッジに合わせ、board[x][y]かつ、x右向き、y下向き
  array<array<int, 4>, 4> board;
  // 評価関数による盤面評価値
  ll evaluated_score = 0;
  // 探索木のルートノードからの最初の行動
  int first_action = -1;

  State() { rep(i, 4) rep(j, 4) board[i][j] = 0; }

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

  void evaluate_score() {
    // 盤上の空スペースの個数
    int cnt_0 = 0;
    rep(y, 4) rep(x, 4) if (board[y][x] == 0) cnt_0++;

    evaluated_score = cnt_0;
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
// 探索時のソート用に評価を比較する
bool operator<(const State &state_1, const State &state_2) {
  return state_1.evaluated_score < state_2.evaluated_score;
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

string decide_action_beam_search_multi(State &init_state, TimeKeeper &tk,
                                       const int beam_width,
                                       const int multi_turn) {
  vector<pair<string, State>> init_legal_actions;
  auto dfs = [&](auto rec, string action = "", int depth = 0) -> void {
    if (depth == multi_turn) {
      State new_state = init_state;
      string s;
      for (auto c : action) {
        vector<int> vec = new_state.get_legal_actions();
        if (find(all(vec), char2action[c]) == vec.end()) {
          break;
        }
        s.push_back(c);
        new_state.advance(char2action[c]);
      }
      if (s != "") {
        init_legal_actions.emplace_back(s, new_state);
      }
      return;
    }
    rec(rec, action + "U", depth + 1);
    rec(rec, action + "D", depth + 1);
    rec(rec, action + "L", depth + 1);
    rec(rec, action + "R", depth + 1);
  };
  dfs(dfs);
  //
  priority_queue<State> now_beam;
  int best_idx = 0;

  if (init_legal_actions.empty()) return "";
  rep(i, init_legal_actions.size()) {
    State &state = init_legal_actions[i].second;
    state.first_action = i;
    now_beam.push(state);
  }

  int t = 0;
  for (;; t++) {
    priority_queue<State> next_beam;
    rep(_, beam_width) {
      if (tk.isTimeOver()) break;
      if (now_beam.empty()) break;
      State now_state = now_beam.top();
      now_beam.pop();
      auto legal_actions = now_state.get_legal_actions();
      for (const auto &action : legal_actions) {
        State next_state = now_state;
        next_state.advance(action);
        next_state.evaluate_score();
        next_state.first_action = now_state.first_action;
        next_beam.push(next_state);
      }
    }
    if (tk.isTimeOver()) break;
    now_beam = next_beam;
    if (now_beam.empty()) break;
    best_idx = now_beam.top().first_action;
  }
  return init_legal_actions[best_idx].first;
}

// 動作確認用
void local_play() {
  State state;
  state.board[1][3] = 4;
  state.board[3][1] = 2;
  state.score = 0;
  state.seed = 13339144;

  int turn = 0;
  int depth = 3;
  while (1) {
    TimeKeeper tk(40);
    turn++;
    // if (turn == 200) depth++;
    // if (turn == 400) depth++;
    // if (turn == 500) depth++;
    // if (turn == 550) depth++;
    string actions = decide_action_beam_search_multi(state, tk, 10, depth);
    if (actions == "") break;
    for (char c : actions) {
      state.advance(char2action[c]);
      state.debug_board();
      cerr << c << endl;
    }
  }
  state.debug_board();
}

int main() {
  // test();
  // local_play();
  // return 0;

  // game loop
  int turn = 1;
  int depth = 2;
  int _;

  State state;
  state.build();
  {
    TimeKeeper tk(900);
    string actions = decide_action_beam_search_multi(state, tk, 20, 3);
    for (char c : actions) {
      state.advance(char2action[c]);
    }
    cout << actions << endl;
  }
  while (1) {
    rep(i, 18) cin >> _;
    turn++;
    TimeKeeper tk(40);
    if (turn == 400) depth++;
    string actions = decide_action_beam_search_multi(state, tk, 20, depth);
    for (char c : actions) {
      state.advance(char2action[c]);
    }
    if (actions == "") break;
    cout << actions << endl;
  }
}