#include "score_helpers.h"
#include <QRandomGenerator>

bool ScoreHelpers::isAllowed(FillMode fill, int, int activeRow, int activeCol, int r, int c) const {
    switch (fill) {
        case FillMode::Free:
            return true;
        case FillMode::TopDownRows:
        case FillMode::RandomRow:
            return r == activeRow;
        case FillMode::LeftRightCols:
        case FillMode::RandomCol:
            return c == activeCol;
        case FillMode::RandomRowOrCol:
            return (r == activeRow) || (c == activeCol);
        case FillMode::Gravity:
            return true;
    }
    return true;
}

int ScoreHelpers::pieceCost(int player, int moveCountForThatPlayer) const {
    if (moveCountForThatPlayer < 1) moveCountForThatPlayer = 1;

    if (player == 1) {
        return 2 * moveCountForThatPlayer;
    }
    return 2 * moveCountForThatPlayer + 1;
}

static bool inBounds(int N, int r, int c) {
    return r >= 0 && c >= 0 && r < N && c < N;
}

int ScoreHelpers::lineDelta(const QVector<int>& board,
                            const QVector<int>& weights,
                            int N, int r, int c,
                            int L, int player) const {
    int delta = 0;

    const int dirs[4][2] = { {0,1}, {1,0}, {1,1}, {1,-1} };

    for (int di = 0; di < 4; ++di) {
        const int dr = dirs[di][0];
        const int dc = dirs[di][1];

        for (int off = -(L - 1); off <= 0; ++off) {
            const int sr = r + off * dr;
            const int sc = c + off * dc;
            const int er = sr + (L - 1) * dr;
            const int ec = sc + (L - 1) * dc;

            if (!inBounds(N, sr, sc) || !inBounds(N, er, ec)) continue;

            bool ok = true;
            int sumW = 0;
            for (int k = 0; k < L; ++k) {
                const int rr = sr + k * dr;
                const int cc = sc + k * dc;
                const int idx = rr * N + cc;
                if (board[idx] != player) { ok = false; break; }
                sumW += weights[idx];
            }
            if (ok) delta += sumW;
        }
    }

    return delta;
}

bool ScoreHelpers::rowHasEmpty(const QVector<int>& board, int N, int r) const {
    for (int c = 0; c < N; ++c) {
        if (board[r * N + c] == 0) return true;
    }
    return false;
}

bool ScoreHelpers::colHasEmpty(const QVector<int>& board, int N, int c) const {
    for (int r = 0; r < N; ++r) {
        if (board[r * N + c] == 0) return true;
    }
    return false;
}

int ScoreHelpers::pickRandomRowWithEmpty(const QVector<int>& board, int N) const {
    QVector<int> rows;
    rows.reserve(N);
    for (int r = 0; r < N; ++r) {
        if (rowHasEmpty(board, N, r)) rows.push_back(r);
    }
    if (rows.isEmpty()) return -1;
    const int idx = QRandomGenerator::global()->bounded(rows.size());
    return rows[idx];
}

int ScoreHelpers::pickRandomColWithEmpty(const QVector<int>& board, int N) const {
    QVector<int> cols;
    cols.reserve(N);
    for (int c = 0; c < N; ++c) {
        if (colHasEmpty(board, N, c)) cols.push_back(c);
    }
    if (cols.isEmpty()) return -1;
    const int idx = QRandomGenerator::global()->bounded(cols.size());
    return cols[idx];
}

void ScoreHelpers::updateStripe(FillMode fill,
                                const QVector<int>& board,
                                int N,
                                int& activeRow,
                                int& activeCol) const {
    switch (fill) {
        case FillMode::Free:
        case FillMode::Gravity:
            activeRow = -1;
            activeCol = -1;
            return;

        case FillMode::TopDownRows: {
            int r = activeRow;
            if (r < 0) r = 0;
            while (r < N && !rowHasEmpty(board, N, r)) r++;
            activeRow = (r < N) ? r : -1;
            activeCol = -1;
            return;
        }

        case FillMode::LeftRightCols: {
            int c = activeCol;
            if (c < 0) c = 0;
            while (c < N && !colHasEmpty(board, N, c)) c++;
            activeCol = (c < N) ? c : -1;
            activeRow = -1;
            return;
        }

        case FillMode::RandomRow:
            if (activeRow < 0 || !rowHasEmpty(board, N, activeRow)) {
                activeRow = pickRandomRowWithEmpty(board, N);
            }
            activeCol = -1;
            return;

        case FillMode::RandomCol:
            if (activeCol < 0 || !colHasEmpty(board, N, activeCol)) {
                activeCol = pickRandomColWithEmpty(board, N);
            }
            activeRow = -1;
            return;

        case FillMode::RandomRowOrCol:
            if (activeRow < 0 || !rowHasEmpty(board, N, activeRow)) {
                activeRow = pickRandomRowWithEmpty(board, N);
            }
            if (activeCol < 0 || !colHasEmpty(board, N, activeCol)) {
                activeCol = pickRandomColWithEmpty(board, N);
            }
            return;
    }
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
