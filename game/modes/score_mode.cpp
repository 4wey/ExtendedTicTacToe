#include "score_mode.h"

ScoreMode::ScoreMode() {
    board_.resize(cfg_.boardSize * cfg_.boardSize);
    weights_.resize(cfg_.boardSize * cfg_.boardSize);
    startNewGame();
}

void ScoreMode::rebuildWeights() {
    weights_ = ScoreHelpers::generateWeightsTiled4(cfg_.boardSize);
}

void ScoreMode::startNewGame() {
    for (int i = 0; i < board_.size(); ++i) board_[i] = 0;

    rebuildWeights();

    active_ = true;
    player_ = 1;
    movesMade_ = 0;
    xMoves_ = 0;
    oMoves_ = 0;

    score_ = ScoreSnapshot{};
    activeRow_ = -1;
    activeCol_ = -1;

    updateStripe();
}

std::unique_ptr<IGameMode> ScoreMode::clone() const {
    return std::make_unique<ScoreMode>(*this);
}

int ScoreMode::cellOwner(int r, int c) const {
    const int N = cfg_.boardSize;
    if (r < 0 || c < 0 || r >= N || c >= N) return 0;
    return board_[r * N + c];
}

int ScoreMode::cellWeight(int r, int c) const {
    const int N = cfg_.boardSize;
    if (r < 0 || c < 0 || r >= N || c >= N) return 0;
    return weights_[r * N + c];
}

bool ScoreMode::isMoveAllowed(int r, int c) const {
    if (!active_) return false;

    const int N = cfg_.boardSize;
    if (r < 0 || c < 0 || r >= N || c >= N) return false;
    if (board_[r * N + c] != 0) return false;

    if (fill_ == FillMode::Gravity) {
        if (r < N - 1 && board_[(r + 1) * N + c] == 0) return false;
    }

    return helpers_.isAllowed(fill_, N, activeRow_, activeCol_, r, c);
}

void ScoreMode::updateStripe() {
    helpers_.updateStripe(fill_, board_, cfg_.boardSize, activeRow_, activeCol_);
}

MoveOutcome ScoreMode::applyMove(int r, int c) {
    MoveOutcome out{};
    if (!isMoveAllowed(r, c)) return out;

    const int N = cfg_.boardSize;
    const int idx = r * N + c;

    board_[idx] = player_;
    movesMade_++;
    out.accepted = true;

    if (player_ == 1) {
        xMoves_++;
        score_.xSpent += (cellWeight(r, c) + helpers_.pieceCost(1, xMoves_));
        score_.xLine += helpers_.lineDelta(board_, weights_, N, r, c, cfg_.winLine, 1);
    } else {
        oMoves_++;
        score_.oSpent += (cellWeight(r, c) + helpers_.pieceCost(-1, oMoves_));
        score_.oLine += helpers_.lineDelta(board_, weights_, N, r, c, cfg_.winLine, -1);
    }

    score_.xTotal = score_.xLine - score_.xSpent;
    score_.oTotal = score_.oLine - score_.oSpent;

    out.score = score_;

    if (movesLeft() <= 0) {
        active_ = false;
        out.finished = true;
        return out;
    }

    player_ = -player_;
    updateStripe();
    return out;
}
