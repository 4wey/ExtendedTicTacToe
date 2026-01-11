#pragma once

#include "game/game_types.h"
#include <QVector>

class ScoreHelpers {
public:
    ScoreHelpers() = default;
    static QVector<int> generateWeightsTiled4(int N);
    void setFillMode(FillMode fill);
    FillMode fillMode() const { return fillMode_; }
    void reset(const QVector<int>& state, int N);
    void onMoveApplied(const QVector<int>& state, int N);

    bool isMoveAllowed(const QVector<int>& state, int N, int r, int c) const;

    int activeRow(const QVector<int>& state, int N) const;
    int activeCol(const QVector<int>& state, int N) const;

private:
    int firstRowWithEmpty(const QVector<int>& state, int N) const;
    int firstColWithEmpty(const QVector<int>& state, int N) const;

    int randomRowWithEmpty(const QVector<int>& state, int N);
    int randomColWithEmpty(const QVector<int>& state, int N);

    void ensureRandomValid(const QVector<int>& state, int N);
    void rerollRandom(const QVector<int>& state, int N);

private:
    FillMode fillMode_ = FillMode::Free;
    int activeRow_ = -1;
    int activeCol_ = -1;
};
