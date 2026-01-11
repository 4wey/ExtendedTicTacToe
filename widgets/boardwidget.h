#pragma once

#include <QWidget>
#include <QVector>
#include <QIcon>

class QGridLayout;
class QPushButton;

class BoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit BoardWidget(QWidget* parent = nullptr);

    void setBoardSize(int n);

    // owner:  1 = X, -1 = O, 0 = пусто
    void setCellOwner(int r, int c, int owner, bool enabled);

    // веса (для score-режима)
    void setCellWeight(int r, int c, int weight);
    void setWeights(const QVector<int>& weights); // size = N*N
    void setShowWeights(bool on);
    bool showWeights() const { return showWeights_; }

    // фиксированный размер клетки
    void setCellSizePx(int px);
    int cellSizePx() const { return cellSizePx_; }

    // иконки (если у тебя уже выставляются где-то — оставь, но этот метод удобный)
    void setIcons(const QIcon& xIcon, const QIcon& oIcon);

signals:
    void cellClicked(int r, int c);

private:
    void rebuild();
    bool inBounds(int r, int c) const { return r >= 0 && c >= 0 && r < N_ && c < N_; }
    int idx(int r, int c) const { return r * N_ + c; }

private:
    int N_ = 3;

    QGridLayout* grid_ = nullptr;

    QVector<QPushButton*> buttons_;
    QVector<int> owners_;   // 1 / -1 / 0
    QVector<int> weights_;  // вес клетки

    QIcon xIcon_;
    QIcon oIcon_;

    bool showWeights_ = false;
    int cellSizePx_ = 96;
};
