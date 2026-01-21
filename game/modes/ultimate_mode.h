#pragma once

#include "igame_mode.h"
#include <QVector>

class UltimateMode : public IGameMode {
public:
UltimateMode();

GameMode mode() const override { return GameMode::Ultimate; }

void startNewGame() override;
bool isActive() const override { return active_; }

int boardSize() const override { return cfg_.boardSize; }
int movesMade() const override { return movesMade_; }
int movesLeft() const override {
    if (!active_) return 0;
    int left = 0;
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (board_[r * 9 + c] != 0) continue;
            const int localIdx = localIndexForCell(r, c);
            if (!localPlayable(localIdx)) continue;
            left++;
        }
    }
    return left;
}
int currentPlayer() const override { return currentPlayer_; }

int cellOwner(int r, int c) const override;
int cellWeight(int, int) const override { return 0; }

bool isMoveAllowed(int r, int c) const override;
MoveOutcome applyMove(int r, int c) override;

int activeRow() const override;
int activeCol() const override;

FillMode fillMode() const override { return FillMode::Free; }
void setFillMode(FillMode) override {}

ScoreSnapshot currentScore() const override { return ScoreSnapshot{}; }

GameConfig config() const override { return cfg_; }

std::unique_ptr<IGameMode> clone() const override;


private:
int localIndexForCell(int r, int c) const;
int localWinner(int localIdx) const;
bool localFull(int localIdx) const;
bool localPlayable(int localIdx) const;
int macroWinner() const;

private:
GameConfig cfg_;
bool active_ = false;
int currentPlayer_ = 1;
int movesMade_ = 0;

QVector<int> board_;
QVector<int> localState_;

int forcedLocal_ = -1;


};