#include "game_engine.h"

#include "game/modes/classic_mode.h"
#include "game/modes/score_mode.h"

#include <limits>

static int scoreDiffForPlayer(const ScoreSnapshot& s, int player) {
    return (player == 1) ? (s.xTotal - s.oTotal) : (s.oTotal - s.xTotal);
}

static int classicTerminalScore(int winner, int aiPlayer, int depth) {
    if (winner == 0) return 0;
    if (winner == aiPlayer) return 10 - depth;
    return -10 + depth;
}

static int classicMinimaxValue(const IGameMode& state, int aiPlayer, int depth) {
    const bool maximizing = (state.currentPlayer() == aiPlayer);

    int best = maximizing ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    bool any = false;

    const int N = state.boardSize();
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            if (!state.isMoveAllowed(r, c)) continue;
            any = true;

            std::unique_ptr<IGameMode> child = state.clone();
            MoveOutcome out = child->applyMove(r, c);

            int val = 0;
            if (out.finished) {
                val = classicTerminalScore(out.classicWinner, aiPlayer, depth);
            } else {
                val = classicMinimaxValue(*child, aiPlayer, depth + 1);
            }

            if (maximizing) {
                if (val > best) best = val;
            } else {
                if (val < best) best = val;
            }
        }
    }

    if (!any) return 0;
    return best;
}

static bool pickBestClassicMove(const IGameMode& state, int aiPlayer, int& outR, int& outC) {
    int bestVal = std::numeric_limits<int>::min();
    bool found = false;

    const int N = state.boardSize();
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            if (!state.isMoveAllowed(r, c)) continue;

            std::unique_ptr<IGameMode> child = state.clone();
            MoveOutcome out = child->applyMove(r, c);

            int val = 0;
            if (out.finished) {
                val = classicTerminalScore(out.classicWinner, aiPlayer, 1);
            } else {
                val = classicMinimaxValue(*child, aiPlayer, 2);
            }

            if (!found || val > bestVal) {
                found = true;
                bestVal = val;
                outR = r;
                outC = c;
            }
        }
    }

    return found;
}

static bool pickBestScoreMoveDepth2(const IGameMode& state, int aiPlayer, int& outR, int& outC) {
    int bestVal = std::numeric_limits<int>::min();
    bool found = false;

    const int N = state.boardSize();

    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            if (!state.isMoveAllowed(r, c)) continue;

            std::unique_ptr<IGameMode> afterMe = state.clone();
            MoveOutcome out1 = afterMe->applyMove(r, c);

            int val = 0;
            if (out1.finished) {
                val = scoreDiffForPlayer(out1.score, aiPlayer);
            } else {
                int worstForMe = std::numeric_limits<int>::max();
                bool oppFound = false;

                for (int rr = 0; rr < N; ++rr) {
                    for (int cc = 0; cc < N; ++cc) {
                        if (!afterMe->isMoveAllowed(rr, cc)) continue;

                        oppFound = true;
                        std::unique_ptr<IGameMode> afterOpp = afterMe->clone();
                        MoveOutcome out2 = afterOpp->applyMove(rr, cc);

                        int d = scoreDiffForPlayer(out2.score, aiPlayer);
                        if (d < worstForMe) worstForMe = d;
                    }
                }

                val = oppFound ? worstForMe : scoreDiffForPlayer(out1.score, aiPlayer);
            }

            if (!found || val > bestVal) {
                found = true;
                bestVal = val;
                outR = r;
                outC = c;
            }
        }
    }

    return found;
}

// ===================== GameEngine =====================

GameEngine::GameEngine() {
    setMode(GameMode::Classic3x3);
}

void GameEngine::setMode(GameMode mode) {
    mode_ = mode;

    if (mode_ == GameMode::Classic3x3) {
        modeImpl_ = std::make_unique<ClassicMode>();
    } else {
        modeImpl_ = std::make_unique<ScoreMode>();
    }
}

void GameEngine::setFillMode(FillMode fill) {
    if (!modeImpl_) return;
    modeImpl_->setFillMode(fill);
}

void GameEngine::startNewGame() {
    if (!modeImpl_) return;
    modeImpl_->startNewGame();
}

MoveOutcome GameEngine::applyMove(int r, int c) {
    if (!modeImpl_) return MoveOutcome{};
    return modeImpl_->applyMove(r, c);
}

bool GameEngine::isCurrentPlayerComputer() const {
    if (!modeImpl_ || !modeImpl_->isActive()) return false;
    const int p = modeImpl_->currentPlayer();
    if (p == 1) return xType_ == PlayerType::Computer;
    return oType_ == PlayerType::Computer;
}

bool GameEngine::pickComputerMove(int& outR, int& outC) const {
    if (!modeImpl_) return false;

    const int aiPlayer = modeImpl_->currentPlayer();
    if (modeImpl_->mode() == GameMode::Classic3x3) {
        return pickBestClassicMove(*modeImpl_, aiPlayer, outR, outC);
    }

    return pickBestScoreMoveDepth2(*modeImpl_, aiPlayer, outR, outC);
}

MoveOutcome GameEngine::doComputerMove() {
    MoveOutcome out;
    out.accepted = false;

    if (!modeImpl_ || !modeImpl_->isActive()) return out;
    if (!isCurrentPlayerComputer()) return out;

    int r = -1, c = -1;
    if (!pickComputerMove(r, c)) return out;

    return modeImpl_->applyMove(r, c);
}
