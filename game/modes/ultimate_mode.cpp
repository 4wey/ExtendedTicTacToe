#include "ultimate_mode.h"

UltimateMode::UltimateMode() {
cfg_.mode = GameMode::Ultimate;
cfg_.boardSize = 9;
cfg_.winLine = 3;
cfg_.stripeThickness = 3;
cfg_.maxMoves = 81;

board_.resize(cfg_.boardSize * cfg_.boardSize);
localState_.resize(9);

startNewGame();


}

std::unique_ptr<IGameMode> UltimateMode::clone() const {
return std::make_unique<UltimateMode>(*this);
}

void UltimateMode::startNewGame() {
active_ = true;
currentPlayer_ = 1;
movesMade_ = 0;
forcedLocal_ = -1;

for (int i = 0; i < board_.size(); ++i) board_[i] = 0;
for (int i = 0; i < localState_.size(); ++i) localState_[i] = 0;


}

int UltimateMode::cellOwner(int r, int c) const {
const int N = cfg_.boardSize;
if (r < 0 || c < 0 || r >= N || c >= N) return 0;
return board_[r * N + c];
}

int UltimateMode::localIndexForCell(int r, int c) const {
return (r / 3) * 3 + (c / 3);
}

int UltimateMode::localWinner(int localIdx) const {
if (localIdx < 0 || localIdx >= 9) return 0;

const int baseR = (localIdx / 3) * 3;
const int baseC = (localIdx % 3) * 3;

auto at = [&](int rr, int cc) -> int {
    return board_[(baseR + rr) * 9 + (baseC + cc)];
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

bool UltimateMode::localFull(int localIdx) const {
if (localIdx < 0 || localIdx >= 9) return true;

const int baseR = (localIdx / 3) * 3;
const int baseC = (localIdx % 3) * 3;

for (int rr = 0; rr < 3; ++rr) {
    for (int cc = 0; cc < 3; ++cc) {
        if (board_[(baseR + rr) * 9 + (baseC + cc)] == 0) return false;
    }
}

return true;


}

bool UltimateMode::localPlayable(int localIdx) const {
if (localIdx < 0 || localIdx >= 9) return false;
if (localState_[localIdx] != 0) return false;
return !localFull(localIdx);
}

int UltimateMode::macroWinner() const {
auto macroAt = [&](int r, int c) -> int {
const int idx = r * 3 + c;
if (idx < 0 || idx >= 9) return 0;
const int v = localState_[idx];
if (v == 2) return 0;
return v;
};

for (int r = 0; r < 3; ++r) {
    int s = macroAt(r, 0) + macroAt(r, 1) + macroAt(r, 2);
    if (s == 3) return 1;
    if (s == -3) return -1;
}

for (int c = 0; c < 3; ++c) {
    int s = macroAt(0, c) + macroAt(1, c) + macroAt(2, c);
    if (s == 3) return 1;
    if (s == -3) return -1;
}

int d1 = macroAt(0, 0) + macroAt(1, 1) + macroAt(2, 2);
int d2 = macroAt(0, 2) + macroAt(1, 1) + macroAt(2, 0);

if (d1 == 3 || d2 == 3) return 1;
if (d1 == -3 || d2 == -3) return -1;

return 0;


}

bool UltimateMode::isMoveAllowed(int r, int c) const {
if (!active_) return false;

const int N = cfg_.boardSize;
if (r < 0 || c < 0 || r >= N || c >= N) return false;
if (board_[r * N + c] != 0) return false;

const int localIdx = localIndexForCell(r, c);
if (!localPlayable(localIdx)) return false;

int effectiveForced = forcedLocal_;
if (effectiveForced != -1 && !localPlayable(effectiveForced)) effectiveForced = -1;


if (effectiveForced == -1) return true;
return localIdx == effectiveForced;


}

MoveOutcome UltimateMode::applyMove(int r, int c) {
MoveOutcome out{};
if (!isMoveAllowed(r, c)) return out;

const int N = cfg_.boardSize;
board_[r * N + c] = currentPlayer_;
movesMade_++;

out.accepted = true;

const int localIdx = localIndexForCell(r, c);
int lw = localWinner(localIdx);
if (lw != 0) {
    localState_[localIdx] = lw;
} else if (localFull(localIdx)) {
    localState_[localIdx] = 2;
}

const int mw = macroWinner();
if (mw != 0) {
    active_ = false;
    out.finished = true;
    out.classicWinner = mw;
    return out;
}

bool anyPlayable = false;
for (int i = 0; i < 9; ++i) {
    if (localPlayable(i)) {
        anyPlayable = true;
        break;
    }
}

if (!anyPlayable) {
    active_ = false;
    out.finished = true;
    out.classicWinner = 0;
    return out;
}

const int nextLocal = (r % 3) * 3 + (c % 3);
forcedLocal_ = localPlayable(nextLocal) ? nextLocal : -1;

currentPlayer_ = -currentPlayer_;
return out;


}

int UltimateMode::activeRow() const {
if (forcedLocal_ == -1) return -1;
if (!localPlayable(forcedLocal_)) return -1;
return forcedLocal_ / 3;
}

int UltimateMode::activeCol() const {
if (forcedLocal_ == -1) return -1;
if (!localPlayable(forcedLocal_)) return -1;
return forcedLocal_ % 3;
}
