#include "classic_mode.h"

ClassicMode::ClassicMode() {
    cfg_.mode = GameMode::Classic3x3;
    cfg_.boardSize = 3;
    cfg_.winLine = 3;
    cfg_.stripeThickness = 1;
    cfg_.maxMoves = 9;

    board_.resize(cfg_.boardSize * cfg_.boardSize);
    startNewGame();
}

std::unique_ptr<IGameMode> ClassicMode::clone() const {
    return std::make_unique<ClassicMode>(*this);
}

void ClassicMode::startNewGame() {
    active_ = true;
    currentPlayer_ = 1;
    movesMade_ = 0;
    for (int i = 0; i < board_.size(); ++i) board_[i] = 0;
}

int ClassicMode::cellOwner(int r, int c) const {
    const int N = cfg_.boardSize;
    return board_[r * N + c];
}

bool ClassicMode::isMoveAllowed(int r, int c) const {
    if (!active_) return false;
    const int N = cfg_.boardSize;
    if (r < 0 || c < 0 || r >= N || c >= N) return false;
    return cellOwner(r, c) == 0;
}

MoveOutcome ClassicMode::applyMove(int r, int c) {
    MoveOutcome out;
    if (!isMoveAllowed(r, c)) return out;

    const int N = cfg_.boardSize;
    board_[r * N + c] = currentPlayer_;
    movesMade_++;

    out.accepted = true;

    const int winner = checkWinner();
    if (winner != 0) {
        active_ = false;
        out.finished = true;
        out.classicWinner = winner;
        return out;
    }

    if (movesMade_ >= N * N) {
        active_ = false;
        out.finished = true;
        out.classicWinner = 0;
        return out;
    }

    currentPlayer_ = -currentPlayer_;
    return out;
}

int ClassicMode::checkWinner() const {
    const int N = cfg_.boardSize;

    auto at = [&](int r, int c) -> int { return board_[r * N + c]; };

    for (int r = 0; r < N; ++r) {
        int s = 0;
        for (int c = 0; c < N; ++c) s += at(r, c);
        if (s == N) return 1;
        if (s == -N) return -1;
    }

    for (int c = 0; c < N; ++c) {
        int s = 0;
        for (int r = 0; r < N; ++r) s += at(r, c);
        if (s == N) return 1;
        if (s == -N) return -1;
    }

    int s1 = 0, s2 = 0;
    for (int i = 0; i < N; ++i) {
        s1 += at(i, i);
        s2 += at(i, N - 1 - i);
    }
    if (s1 == N || s2 == N) return 1;
    if (s1 == -N || s2 == -N) return -1;

    return 0;
}
