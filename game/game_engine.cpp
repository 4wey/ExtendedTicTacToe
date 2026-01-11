#include "game_engine.h"
#include "game/modes/classic_mode.h"
#include "game/modes/score_mode.h"

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


int GameEngine::cellWeight(int r, int c) const {
    if (!modeImpl_) return 0;
    return modeImpl_->cellWeight(r, c);
}
