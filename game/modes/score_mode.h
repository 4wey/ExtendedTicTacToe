#pragma once

#include "game/modes/igame_mode.h"
#include "game/score/score_helpers.h"
#include <QVector>

class ScoreMode : public IGameMode {
public:
    ScoreMode();

    GameMode mode() const override { return GameMode::Score10x10; }
    GameConfig config() const override { return cfg_; }

    void setFillMode(FillMode fill) override { fill_ = fill; }
    FillMode fillMode() const override { return fill_; }
    std::unique_ptr<IGameMode> clone() const override;

    void startNewGame() override;
    bool isActive() const override { return active_; }

    int boardSize() const override { return cfg_.boardSize; }
    int movesMade() const override { return movesMade_; }
    int movesLeft() const override {
        const int maxMoves = (cfg_.maxMoves > 0) ? cfg_.maxMoves : (cfg_.boardSize * cfg_.boardSize);
        const int left = maxMoves - movesMade_;
        return (left < 0) ? 0 : left;
    }

    int currentPlayer() const override { return player_; }
    int cellOwner(int r, int c) const override;
    int cellWeight(int r, int c) const override;

    bool isMoveAllowed(int r, int c) const override;
    MoveOutcome applyMove(int r, int c) override;

    int activeRow() const override { return activeRow_; }
    int activeCol() const override { return activeCol_; }

    ScoreSnapshot currentScore() const override { return score_; }

private:
    void updateStripe();
    void rebuildWeights();

private:
    GameConfig cfg_{
    GameMode::Score10x10,
    10,
    4,
    60
    };
    bool active_ = false;

    int player_ = 1;
    int movesMade_ = 0;

    int xMoves_ = 0;
    int oMoves_ = 0;

    QVector<int> board_;
    QVector<int> weights_;

    FillMode fill_ = FillMode::Free;
    int activeRow_ = -1;
    int activeCol_ = -1;

    ScoreSnapshot score_{};
    ScoreHelpers helpers_{};
};
