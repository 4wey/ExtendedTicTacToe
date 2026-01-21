#pragma once

#include "igame_mode.h"

#include <QVector>

class ClassicMode : public IGameMode {
public:
    ClassicMode();

    GameMode mode() const override { return GameMode::Classic3x3; }

    void startNewGame() override;
    bool isActive() const override { return active_; }

    int boardSize() const override { return cfg_.boardSize; }
    int movesMade() const override { return movesMade_; }
    int movesLeft() const override { return cfg_.boardSize * cfg_.boardSize - movesMade_; }

    int currentPlayer() const override { return currentPlayer_; }

    int cellOwner(int r, int c) const override;
    int cellWeight(int, int) const override { return 0; }

    bool isMoveAllowed(int r, int c) const override;
    MoveOutcome applyMove(int r, int c) override;

    int activeRow() const override { return -1; }
    int activeCol() const override { return -1; }

    FillMode fillMode() const override { return FillMode::Free; }
    void setFillMode(FillMode) override {}

    ScoreSnapshot currentScore() const override { return ScoreSnapshot{}; }

    GameConfig config() const override { return cfg_; }
    std::unique_ptr<IGameMode> clone() const override;

private:
    int checkWinner() const;

private:
    GameConfig cfg_;
    bool active_ = false;
    int currentPlayer_ = 1;
    int movesMade_ = 0;

    QVector<int> board_;
};
