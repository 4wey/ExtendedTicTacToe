#include "classic_mode.h"

static bool inBounds(int N, int r, int c) {
    return r >= 0 && c >= 0 && r < N && c < N;
}

static int countDir(const QVector<int>& s, int N, int r, int c, int dr, int dc, int player) {
    int cnt = 0;
    while (inBounds(N, r, c) && s[r * N + c] == player) {
        cnt++;
        r += dr;
        c += dc;
    }
    return cnt;
}

ClassicMode::ClassicMode() {
    state_.resize(cfg_.boardSize * cfg_.boardSize);
    for (int i = 0; i < state_.size(); ++i) state_[i] = 0;
}

void ClassicMode::startNewGame() {
    for (int i = 0; i < state_.size(); ++i) state_[i] = 0;
    active_ = true;
    player_ = 1;
    movesMade_ = 0;
}

int ClassicMode::cellOwner(int r, int c) const {
    if (!inBounds(cfg_.boardSize, r, c)) return 0;
    return state_[r * cfg_.boardSize + c];
}

bool ClassicMode::isMoveAllowed(int r, int c) const {
    if (!active_) return false;
    if (!inBounds(cfg_.boardSize, r, c)) return false;
    return state_[r * cfg_.boardSize + c] == 0;
}

bool ClassicMode::checkWinFrom(int r, int c, int player) const {
    const int N = cfg_.boardSize;
    const int L = cfg_.winLen;
    const int dirs[4][2] = { {0,1}, {1,0}, {1,1}, {1,-1} };

    for (int i = 0; i < 4; ++i) {
        int dr = dirs[i][0], dc = dirs[i][1];
        int total = countDir(state_, N, r, c, dr, dc, player)
                  + countDir(state_, N, r, c, -dr, -dc, player) - 1;
        if (total >= L) return true;
    }
    return false;
}

MoveOutcome ClassicMode::applyMove(int r, int c) {
    MoveOutcome out{};
    if (!isMoveAllowed(r, c)) return out;

    int idx = r * cfg_.boardSize + c;
    state_[idx] = player_;
    movesMade_++;
    out.accepted = true;

    if (checkWinFrom(r, c, player_)) {
        out.finished = true;
        out.classicWinner = player_;
        active_ = false;
        return out;
    }

    if (movesMade_ >= cfg_.maxMoves) {
        out.finished = true;
        out.classicWinner = 0;
        active_ = false;
        return out;
    }

    player_ *= -1;
    return out;
}
