#pragma once

#include "game/modes/igame_mode.h"
#include <QVector>

class ClassicMode : public IGameMode {
public:
    ClassicMode();

    GameMode mode() const override { return GameMode::Classic3x3; }
    GameConfig config() const override { return cfg_; }
    int cellWeight(int, int) const override { return 0; }

    void setFillMode(FillMode) override {}
    FillMode fillMode() const override { return FillMode::Free; }

    void startNewGame() override;
    bool isActive() const override { return active_; }

    int boardSize() const override { return cfg_.boardSize; }
    int movesMade() const override { return movesMade_; }
    int movesLeft() const override { return cfg_.maxMoves - movesMade_; }

    int currentPlayer() const override { return player_; }
    int cellOwner(int r, int c) const override;

    bool isMoveAllowed(int r, int c) const override;
    MoveOutcome applyMove(int r, int c) override;

    int activeRow() const override { return -1; }
    int activeCol() const override { return -1; }

    ScoreSnapshot currentScore() const override { return {}; }

private:
    bool checkWinFrom(int r, int c, int player) const;

private:
    GameConfig cfg_{3,3,9};
    QVector<int> state_;

    bool active_ = false;
    int player_ = 1;
    int movesMade_ = 0;
};
