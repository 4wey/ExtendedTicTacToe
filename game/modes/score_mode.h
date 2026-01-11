#pragma once

#include "game/modes/igame_mode.h"
#include "game/score/score_helpers.h"
#include <QVector>

class ScoreMode : public IGameMode {
public:
    ScoreMode();

    GameMode mode() const override { return GameMode::Score10x10; }
    GameConfig config() const override { return cfg_; }
    int cellWeight(int r, int c) const override;

    void setFillMode(FillMode fill) override;
    FillMode fillMode() const override { return helpers_.fillMode(); }

    void startNewGame() override;
    bool isActive() const override { return active_; }

    int boardSize() const override { return cfg_.boardSize; }
    int movesMade() const override { return movesMade_; }
    int movesLeft() const override { return cfg_.maxMoves - movesMade_; }

    int currentPlayer() const override { return player_; }
    int cellOwner(int r, int c) const override;

    bool isMoveAllowed(int r, int c) const override;
    MoveOutcome applyMove(int r, int c) override;

    int activeRow() const override;
    int activeCol() const override;

    ScoreSnapshot currentScore() const override;

private:
    int nextMoveCostForPlayer(int player) const;
    int scoreLinesSum(int player) const;

private:
    GameConfig cfg_{10,5,60};

    QVector<int> state_;
    QVector<int> weights_;

    ScoreHelpers helpers_;

    bool active_ = false;
    int player_ = 1;
    int movesMade_ = 0;

    int xMoves_ = 0;
    int oMoves_ = 0;
    int spentX_ = 0;
    int spentO_ = 0;
};
