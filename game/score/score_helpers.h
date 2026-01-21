#pragma once

#include "game/game_types.h"
#include <QVector>

class ScoreHelpers {
public:
    bool isAllowed(FillMode fill, int N, int activeRow, int activeCol, int r, int c) const;

    int pieceCost(int player, int moveCountForThatPlayer) const;

    int lineDelta(const QVector<int>& board,
                  const QVector<int>& weights,
                  int N, int r, int c,
                  int L, int player) const;

    void updateStripe(FillMode fill,
                      const QVector<int>& board,
                      int N,
                      int& activeRow,
                      int& activeCol) const;

    static QVector<int> generateWeightsTiled4(int N);

private:
    bool rowHasEmpty(const QVector<int>& board, int N, int r) const;
    bool colHasEmpty(const QVector<int>& board, int N, int c) const;
    int pickRandomRowWithEmpty(const QVector<int>& board, int N) const;
    int pickRandomColWithEmpty(const QVector<int>& board, int N) const;
};
