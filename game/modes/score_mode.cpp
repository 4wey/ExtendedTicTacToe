#include "score_mode.h"

static bool inBounds(int N, int r, int c) {
    return r >= 0 && c >= 0 && r < N && c < N;
}

ScoreMode::ScoreMode() {
    state_.resize(cfg_.boardSize * cfg_.boardSize);
    for (int i = 0; i < state_.size(); ++i) state_[i] = 0;

    weights_ = ScoreHelpers::generateWeightsTiled4(cfg_.boardSize);
}

void ScoreMode::setFillMode(FillMode fill) {
    helpers_.setFillMode(fill);
    if (active_) helpers_.reset(state_, cfg_.boardSize);
}

void ScoreMode::startNewGame() {
    for (int i = 0; i < state_.size(); ++i) state_[i] = 0;

    weights_ = ScoreHelpers::generateWeightsTiled4(cfg_.boardSize);

    active_ = true;
    player_ = 1;
    movesMade_ = 0;

    xMoves_ = 0;
    oMoves_ = 0;
    spentX_ = 0;
    spentO_ = 0;

    helpers_.reset(state_, cfg_.boardSize);
}

int ScoreMode::cellOwner(int r, int c) const {
    if (!inBounds(cfg_.boardSize, r, c)) return 0;
    return state_[r * cfg_.boardSize + c];
}

bool ScoreMode::isMoveAllowed(int r, int c) const {
    if (!active_) return false;
    return helpers_.isMoveAllowed(state_, cfg_.boardSize, r, c);
}

int ScoreMode::activeRow() const {
    return helpers_.activeRow(state_, cfg_.boardSize);
}

int ScoreMode::activeCol() const {
    return helpers_.activeCol(state_, cfg_.boardSize);
}

int ScoreMode::nextMoveCostForPlayer(int player) const {
    if (player == 1) return 2 * (xMoves_ + 1);
    return 2 * (oMoves_ + 1) + 1;
}

int ScoreMode::scoreLinesSum(int player) const {
    const int N = cfg_.boardSize;
    const int L = cfg_.winLen;
    int total = 0;

    auto addSegment = [&](int startR, int startC, int dr, int dc) {
        int r = startR, c = startC;
        while (inBounds(N, r, c)) {
            int endR = r + (L - 1) * dr;
            int endC = c + (L - 1) * dc;
            if (!inBounds(N, endR, endC)) break;

            bool ok = true;
            int sumW = 0;
            for (int k = 0; k < L; ++k) {
                int rr = r + k * dr;
                int cc = c + k * dc;
                int idx = rr * N + cc;
                if (state_[idx] != player) { ok = false; break; }
                sumW += weights_[idx];
            }
            if (ok) total += sumW;

            r += dr;
            c += dc;
        }
    };

    for (int r = 0; r < N; ++r) addSegment(r, 0, 0, 1);
    for (int c = 0; c < N; ++c) addSegment(0, c, 1, 0);

    for (int c = 0; c < N; ++c) addSegment(0, c, 1, 1);
    for (int r = 1; r < N; ++r) addSegment(r, 0, 1, 1);

    for (int c = 0; c < N; ++c) addSegment(0, c, 1, -1);
    for (int r = 1; r < N; ++r) addSegment(r, N - 1, 1, -1);

    return total;
}

ScoreSnapshot ScoreMode::currentScore() const {
    ScoreSnapshot s{};
    s.xLine = scoreLinesSum(1);
    s.oLine = scoreLinesSum(-1);
    s.xSpent = spentX_;
    s.oSpent = spentO_;
    s.xTotal = s.xLine - s.xSpent;
    s.oTotal = s.oLine - s.oSpent;
    return s;
}

MoveOutcome ScoreMode::applyMove(int r, int c) {
    MoveOutcome out{};
    if (!isMoveAllowed(r, c)) return out;

    const int N = cfg_.boardSize;
    int idx = r * N + c;

    state_[idx] = player_;
    movesMade_++;
    out.accepted = true;

    if (player_ == 1) { xMoves_++; spentX_ += nextMoveCostForPlayer(1); }
    else { oMoves_++; spentO_ += nextMoveCostForPlayer(-1); }

    helpers_.onMoveApplied(state_, N);

    if (movesMade_ >= cfg_.maxMoves) {
        out.finished = true;
        out.score = currentScore();
        active_ = false;
        return out;
    }

    player_ *= -1;
    return out;
}


int ScoreMode::cellWeight(int r, int c) const {
    const int N = cfg_.boardSize;
    if (r < 0 || c < 0 || r >= N || c >= N) return 0;
    return weights_[r * N + c];
}
