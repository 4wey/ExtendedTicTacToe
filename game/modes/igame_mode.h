#pragma once

#include "game/game_types.h"

struct GameConfig {
    int boardSize = 3;
    int winLen = 3;
    int maxMoves = 9;
};

struct ScoreSnapshot {
    int xLine = 0;
    int oLine = 0;
    int xSpent = 0;
    int oSpent = 0;
    int xTotal = 0;
    int oTotal = 0;
};

struct MoveOutcome {
    bool accepted = false;
    bool finished = false;
    int classicWinner = 0;     // 1=X, -1=O, 0=draw/none
    ScoreSnapshot score{};
};

class IGameMode {
public:
    virtual ~IGameMode() = default;

    virtual GameMode mode() const = 0;
    virtual GameConfig config() const = 0;

    virtual void setFillMode(FillMode fill) = 0;
    virtual FillMode fillMode() const = 0;

    virtual void startNewGame() = 0;
    virtual bool isActive() const = 0;

    virtual int boardSize() const = 0;
    virtual int movesMade() const = 0;
    virtual int movesLeft() const = 0;

    virtual int currentPlayer() const = 0; // 1 = X, -1 = O
    virtual int cellOwner(int r, int c) const = 0;

    // ВЕС КЛЕТКИ (для score). В классике возвращай 0.
    virtual int cellWeight(int r, int c) const = 0;

    virtual bool isMoveAllowed(int r, int c) const = 0;
    virtual MoveOutcome applyMove(int r, int c) = 0;

    virtual int activeRow() const = 0;
    virtual int activeCol() const = 0;

    virtual ScoreSnapshot currentScore() const = 0;
};
