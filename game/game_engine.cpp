#include "game_engine.h"

#include "game/modes/classic_mode.h"
#include "game/modes/score_mode.h"
#include "game/modes/ultimate_mode.h"

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

static int ultimateTerminalScore(int winner, int aiPlayer, int depth) {
if (winner == 0) return 0;
if (winner == aiPlayer) return 100000 - depth;
return -100000 + depth;
}

static int localWinner9(const IGameMode& state, int br, int bc) {
auto at = [&](int rr, int cc) -> int {
return state.cellOwner(br * 3 + rr, bc * 3 + cc);
};

for (int r = 0; r < 3; ++r) {
    int s = at(r, 0) + at(r, 1) + at(r, 2);
    if (s == 3) return 1;
    if (s == -3) return -1;
}

for (int c = 0; c < 3; ++c) {
    int s = at(0, c) + at(1, c) + at(2, c);
    if (s == 3) return 1;
    if (s == -3) return -1;
}

int d1 = at(0, 0) + at(1, 1) + at(2, 2);
int d2 = at(0, 2) + at(1, 1) + at(2, 0);

if (d1 == 3 || d2 == 3) return 1;
if (d1 == -3 || d2 == -3) return -1;

return 0;


}

static bool localFull9(const IGameMode& state, int br, int bc) {
for (int rr = 0; rr < 3; ++rr) {
for (int cc = 0; cc < 3; ++cc) {
if (state.cellOwner(br * 3 + rr, bc * 3 + cc) == 0) return false;
}
}
return true;
}

static int macroWinner9(const int localState[9]) {
auto v = [&](int r, int c) -> int {
const int idx = r * 3 + c;
const int s = localState[idx];
if (s == 2) return 0;
return s;
};

for (int r = 0; r < 3; ++r) {
    int s = v(r, 0) + v(r, 1) + v(r, 2);
    if (s == 3) return 1;
    if (s == -3) return -1;
}

for (int c = 0; c < 3; ++c) {
    int s = v(0, c) + v(1, c) + v(2, c);
    if (s == 3) return 1;
    if (s == -3) return -1;
}

int d1 = v(0, 0) + v(1, 1) + v(2, 2);
int d2 = v(0, 2) + v(1, 1) + v(2, 0);

if (d1 == 3 || d2 == 3) return 1;
if (d1 == -3 || d2 == -3) return -1;

return 0;


}

static int ultimateHeuristic(const IGameMode& state, int aiPlayer) {
int localState[9];

for (int br = 0; br < 3; ++br) {
    for (int bc = 0; bc < 3; ++bc) {
        const int idx = br * 3 + bc;
        const int w = localWinner9(state, br, bc);
        if (w != 0) localState[idx] = w;
        else if (localFull9(state, br, bc)) localState[idx] = 2;
        else localState[idx] = 0;
    }
}

const int mw = macroWinner9(localState);
if (mw == aiPlayer) return 90000;
if (mw == -aiPlayer) return -90000;

int score = 0;

for (int i = 0; i < 9; ++i) {
    if (localState[i] == aiPlayer) score += 220;
    else if (localState[i] == -aiPlayer) score -= 240;
    else if (localState[i] == 2) score += 0;
}

const int macroLines[8][3] = {
    {0,1,2},{3,4,5},{6,7,8},
    {0,3,6},{1,4,7},{2,5,8},
    {0,4,8},{2,4,6}
};

for (int k = 0; k < 8; ++k) {
    int sum = 0;
    int empties = 0;
    int blocked = 0;
    for (int j = 0; j < 3; ++j) {
        const int idx = macroLines[k][j];
        if (localState[idx] == 2) {
            blocked++;
            continue;
        }
        sum += localState[idx];
        if (localState[idx] == 0) empties++;
    }

    if (blocked == 0) {
        if (sum == 2 * aiPlayer && empties == 1) score += 700;
        if (sum == -2 * aiPlayer && empties == 1) score -= 900;
        if (sum == aiPlayer && empties == 2) score += 90;
        if (sum == -aiPlayer && empties == 2) score -= 130;
    } else {
        if (sum == 2 * aiPlayer && empties == 1) score += 180;
        if (sum == -2 * aiPlayer && empties == 1) score -= 230;
        if (sum == aiPlayer && empties == 2) score += 25;
        if (sum == -aiPlayer && empties == 2) score -= 35;
    }
}

for (int br = 0; br < 3; ++br) {
    for (int bc = 0; bc < 3; ++bc) {
        const int idx = br * 3 + bc;
        if (localState[idx] != 0) continue;

        auto at = [&](int rr, int cc) -> int {
            return state.cellOwner(br * 3 + rr, bc * 3 + cc);
        };

        for (int r = 0; r < 3; ++r) {
            int a = at(r, 0), b = at(r, 1), c = at(r, 2);
            int sum = a + b + c;
            int empty = (a == 0) + (b == 0) + (c == 0);
            if (sum == 2 * aiPlayer && empty == 1) score += 28;
            if (sum == -2 * aiPlayer && empty == 1) score -= 34;
            if (sum == aiPlayer && empty == 2) score += 4;
            if (sum == -aiPlayer && empty == 2) score -= 6;
        }

        for (int c = 0; c < 3; ++c) {
            int a = at(0, c), b = at(1, c), d = at(2, c);
            int sum = a + b + d;
            int empty = (a == 0) + (b == 0) + (d == 0);
            if (sum == 2 * aiPlayer && empty == 1) score += 28;
            if (sum == -2 * aiPlayer && empty == 1) score -= 34;
            if (sum == aiPlayer && empty == 2) score += 4;
            if (sum == -aiPlayer && empty == 2) score -= 6;
        }

        {
            int a = at(0, 0), b = at(1, 1), c = at(2, 2);
            int sum = a + b + c;
            int empty = (a == 0) + (b == 0) + (c == 0);
            if (sum == 2 * aiPlayer && empty == 1) score += 28;
            if (sum == -2 * aiPlayer && empty == 1) score -= 34;
            if (sum == aiPlayer && empty == 2) score += 4;
            if (sum == -aiPlayer && empty == 2) score -= 6;
        }

        {
            int a = at(0, 2), b = at(1, 1), c = at(2, 0);
            int sum = a + b + c;
            int empty = (a == 0) + (b == 0) + (c == 0);
            if (sum == 2 * aiPlayer && empty == 1) score += 28;
            if (sum == -2 * aiPlayer && empty == 1) score -= 34;
            if (sum == aiPlayer && empty == 2) score += 4;
            if (sum == -aiPlayer && empty == 2) score -= 6;
        }
    }
}

if (localState[4] == aiPlayer) score += 60;
else if (localState[4] == -aiPlayer) score -= 70;

return score;


}

static bool pickBestUltimateMoveDepth2(const IGameMode& state, int aiPlayer, int& outR, int& outC) {
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
            val = ultimateTerminalScore(out1.classicWinner, aiPlayer, 1);
        } else {
            int worstForMe = std::numeric_limits<int>::max();
            bool oppFound = false;

            for (int rr = 0; rr < N; ++rr) {
                for (int cc = 0; cc < N; ++cc) {
                    if (!afterMe->isMoveAllowed(rr, cc)) continue;

                    oppFound = true;
                    std::unique_ptr<IGameMode> afterOpp = afterMe->clone();
                    MoveOutcome out2 = afterOpp->applyMove(rr, cc);

                    int d = 0;
                    if (out2.finished) {
                        d = ultimateTerminalScore(out2.classicWinner, aiPlayer, 2);
                    } else {
                        d = ultimateHeuristic(*afterOpp, aiPlayer);
                    }

                    if (d < worstForMe) worstForMe = d;
                }
            }

            val = oppFound ? worstForMe : ultimateHeuristic(*afterMe, aiPlayer);
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

GameEngine::GameEngine() {
setMode(GameMode::Classic3x3);
}

void GameEngine::setMode(GameMode mode) {
mode_ = mode;

if (mode_ == GameMode::Classic3x3) {
    modeImpl_ = std::make_unique<ClassicMode>();
} else if (mode_ == GameMode::Score10x10) {
    modeImpl_ = std::make_unique<ScoreMode>();
} else {
    modeImpl_ = std::make_unique<UltimateMode>();
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

if (modeImpl_->mode() == GameMode::Score10x10) {
    return pickBestScoreMoveDepth2(*modeImpl_, aiPlayer, outR, outC);
}

return pickBestUltimateMoveDepth2(*modeImpl_, aiPlayer, outR, outC);


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
