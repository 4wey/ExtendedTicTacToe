#pragma once

#include "game/game_types.h"
#include "game/modes/igame_mode.h"
#include <memory>

class GameEngine {
public:
    GameEngine();

    void setMode(GameMode mode);
    void setFillMode(FillMode fill);
    int cellWeight(int r, int c) const;

    GameMode mode() const { return mode_; }
    FillMode fillMode() const { return modeImpl_->fillMode(); }
    GameConfig config() const { return modeImpl_->config(); }

    void startNewGame();
    bool isActive() const { return modeImpl_->isActive(); }

    int boardSize() const { return modeImpl_->boardSize(); }
    int movesMade() const { return modeImpl_->movesMade(); }
    int movesLeft() const { return modeImpl_->movesLeft(); }

    int currentPlayer() const { return modeImpl_->currentPlayer(); }
    int cellOwner(int r, int c) const { return modeImpl_->cellOwner(r, c); }

    bool isMoveAllowed(int r, int c) const { return modeImpl_->isMoveAllowed(r, c); }
    MoveOutcome applyMove(int r, int c) { return modeImpl_->applyMove(r, c); }

    int activeRow() const { return modeImpl_->activeRow(); }
    int activeCol() const { return modeImpl_->activeCol(); }

    ScoreSnapshot currentScore() const { return modeImpl_->currentScore(); }

private:
    GameMode mode_ = GameMode::Classic3x3;
    std::unique_ptr<IGameMode> modeImpl_;
};
