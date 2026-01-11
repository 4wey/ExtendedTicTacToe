#include "score_helpers.h"
#include <QRandomGenerator>

static bool inBounds(int N, int r, int c) {
    return r >= 0 && c >= 0 && r < N && c < N;
}

QVector<int> ScoreHelpers::generateWeightsTiled4(int N) {
    QVector<int> w;
    w.resize(N * N);

    const int tile[4][4] = {
        {  2, -2,  1, -1 },
        { -1,  1, -2,  2 },
        { -2,  2, -1,  1 },
        {  1, -1,  2, -2 }
    };

    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            w[r * N + c] = tile[r % 4][c % 4];
        }
    }
    return w;
}

void ScoreHelpers::setFillMode(FillMode fill) {
    fillMode_ = fill;
    activeRow_ = -1;
    activeCol_ = -1;
}

void ScoreHelpers::reset(const QVector<int>& state, int N) {
    activeRow_ = -1;
    activeCol_ = -1;

    if (fillMode_ == FillMode::RandomRow || fillMode_ == FillMode::RandomCol || fillMode_ == FillMode::RandomRowOrCol) {
        rerollRandom(state, N);
        ensureRandomValid(state, N);
    }
}

void ScoreHelpers::onMoveApplied(const QVector<int>& state, int N) {
    if (fillMode_ == FillMode::RandomRow || fillMode_ == FillMode::RandomCol || fillMode_ == FillMode::RandomRowOrCol) {
        rerollRandom(state, N);
        ensureRandomValid(state, N);
    }
}

int ScoreHelpers::firstRowWithEmpty(const QVector<int>& state, int N) const {
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            if (state[r * N + c] == 0) return r;
        }
    }
    return -1;
}

int ScoreHelpers::firstColWithEmpty(const QVector<int>& state, int N) const {
    for (int c = 0; c < N; ++c) {
        for (int r = 0; r < N; ++r) {
            if (state[r * N + c] == 0) return c;
        }
    }
    return -1;
}

int ScoreHelpers::randomRowWithEmpty(const QVector<int>& state, int N) {
    QVector<int> rows;
    rows.reserve(N);

    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            if (state[r * N + c] == 0) { rows.push_back(r); break; }
        }
    }
    if (rows.isEmpty()) return -1;

    int i = QRandomGenerator::global()->bounded(rows.size());
    return rows[i];
}

int ScoreHelpers::randomColWithEmpty(const QVector<int>& state, int N) {
    QVector<int> cols;
    cols.reserve(N);

    for (int c = 0; c < N; ++c) {
        for (int r = 0; r < N; ++r) {
            if (state[r * N + c] == 0) { cols.push_back(c); break; }
        }
    }
    if (cols.isEmpty()) return -1;

    int i = QRandomGenerator::global()->bounded(cols.size());
    return cols[i];
}

void ScoreHelpers::rerollRandom(const QVector<int>& state, int N) {
    if (fillMode_ == FillMode::RandomRow) {
        activeRow_ = randomRowWithEmpty(state, N);
        activeCol_ = -1;
    } else if (fillMode_ == FillMode::RandomCol) {
        activeCol_ = randomColWithEmpty(state, N);
        activeRow_ = -1;
    } else if (fillMode_ == FillMode::RandomRowOrCol) {
        activeRow_ = randomRowWithEmpty(state, N);
        activeCol_ = randomColWithEmpty(state, N);
    }
}

void ScoreHelpers::ensureRandomValid(const QVector<int>& state, int N) {
    if (fillMode_ == FillMode::RandomRow || fillMode_ == FillMode::RandomRowOrCol) {
        bool ok = false;
        if (activeRow_ >= 0) {
            for (int c = 0; c < N; ++c) {
                if (state[activeRow_ * N + c] == 0) { ok = true; break; }
            }
        }
        if (!ok) activeRow_ = randomRowWithEmpty(state, N);
    }

    if (fillMode_ == FillMode::RandomCol || fillMode_ == FillMode::RandomRowOrCol) {
        bool ok = false;
        if (activeCol_ >= 0) {
            for (int r = 0; r < N; ++r) {
                if (state[r * N + activeCol_] == 0) { ok = true; break; }
            }
        }
        if (!ok) activeCol_ = randomColWithEmpty(state, N);
    }
}

int ScoreHelpers::activeRow(const QVector<int>& state, int N) const {
    if (fillMode_ == FillMode::TopDownRows) return firstRowWithEmpty(state, N);
    if (fillMode_ == FillMode::RandomRow || fillMode_ == FillMode::RandomRowOrCol) return activeRow_;
    return -1;
}

int ScoreHelpers::activeCol(const QVector<int>& state, int N) const {
    if (fillMode_ == FillMode::LeftRightCols) return firstColWithEmpty(state, N);
    if (fillMode_ == FillMode::RandomCol || fillMode_ == FillMode::RandomRowOrCol) return activeCol_;
    return -1;
}

bool ScoreHelpers::isMoveAllowed(const QVector<int>& state, int N, int r, int c) const {
    if (!inBounds(N, r, c)) return false;
    if (state[r * N + c] != 0) return false;

    if (fillMode_ == FillMode::Free) return true;

    if (fillMode_ == FillMode::TopDownRows) {
        int ar = firstRowWithEmpty(state, N);
        return (ar >= 0 && r == ar);
    }

    if (fillMode_ == FillMode::LeftRightCols) {
        int ac = firstColWithEmpty(state, N);
        return (ac >= 0 && c == ac);
    }

    if (fillMode_ == FillMode::RandomRow) return (activeRow_ >= 0 && r == activeRow_);
    if (fillMode_ == FillMode::RandomCol) return (activeCol_ >= 0 && c == activeCol_);
    if (fillMode_ == FillMode::RandomRowOrCol) {
        bool okRow = (activeRow_ >= 0 && r == activeRow_);
        bool okCol = (activeCol_ >= 0 && c == activeCol_);
        return okRow || okCol;
    }

    return true;
}
