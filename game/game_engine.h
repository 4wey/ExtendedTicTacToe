#pragma once

#include "game/game_types.h"
#include "game/modes/igame_mode.h"
#include <memory>

class GameEngine {
public:
    GameEngine();

    void setMode(GameMode mode);
    void setFillMode(FillMode fill);

    void setPlayerTypeX(PlayerType t) { xType_ = t; }
    void setPlayerTypeO(PlayerType t) { oType_ = t; }
    PlayerType playerTypeX() const { return xType_; }
    PlayerType playerTypeO() const { return oType_; }

    void startNewGame();

    bool isActive() const { return modeImpl_ && modeImpl_->isActive(); }

    GameMode mode() const { return mode_; }
    FillMode fillMode() const { return modeImpl_ ? modeImpl_->fillMode() : FillMode::Free; }
    GameConfig config() const { return modeImpl_ ? modeImpl_->config() : GameConfig{}; }

    int boardSize() const { return modeImpl_ ? modeImpl_->boardSize() : 0; }
    int movesMade() const { return modeImpl_ ? modeImpl_->movesMade() : 0; }
    int movesLeft() const { return modeImpl_ ? modeImpl_->movesLeft() : 0; }

    int currentPlayer() const { return modeImpl_ ? modeImpl_->currentPlayer() : 1; } 
    int cellOwner(int r, int c) const { return modeImpl_ ? modeImpl_->cellOwner(r, c) : 0; }
    int cellWeight(int r, int c) const { return modeImpl_ ? modeImpl_->cellWeight(r, c) : 0; }

    bool isMoveAllowed(int r, int c) const { return modeImpl_ ? modeImpl_->isMoveAllowed(r, c) : false; }
    MoveOutcome applyMove(int r, int c);

    int activeRow() const { return modeImpl_ ? modeImpl_->activeRow() : -1; }
    int activeCol() const { return modeImpl_ ? modeImpl_->activeCol() : -1; }

    ScoreSnapshot currentScore() const { return modeImpl_ ? modeImpl_->currentScore() : ScoreSnapshot{}; }

    bool isCurrentPlayerComputer() const;
    MoveOutcome doComputerMove();

private:
    bool pickComputerMove(int& outR, int& outC) const;

private:
    GameMode mode_ = GameMode::Classic3x3;
    PlayerType xType_ = PlayerType::Human;
    PlayerType oType_ = PlayerType::Human;
    std::unique_ptr<IGameMode> modeImpl_;
};
