#include "boardwidget.h"

#include <QGridLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QFont>
#include <QSizePolicy>
#include <QPainter>
#include <algorithm>

namespace {

class CellButton : public QPushButton {
public:
    explicit CellButton(QWidget* parent = nullptr) : QPushButton(parent) {}

    void setWeight(int w) { weight_ = w; update(); }
    int weight() const { return weight_; }

    void setShowWeight(bool on) { showWeight_ = on; update(); }
    bool showWeight() const { return showWeight_; }

protected:
    void paintEvent(QPaintEvent* e) override {
        QPushButton::paintEvent(e);

        if (!showWeight_ || weight_ == 0) return;

        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);

        QFont f = font();
        f.setPointSize(std::max(8, f.pointSize() / 3));
        f.setBold(true);
        p.setFont(f);

        const QString s = (weight_ > 0) ? QString("+%1").arg(weight_) : QString::number(weight_);
        const int pad = 6;
        QRect r = rect().adjusted(pad, pad, -pad, -pad);

        p.drawText(r, Qt::AlignRight | Qt::AlignBottom, s);
    }

private:
    int weight_ = 0;
    bool showWeight_ = false;
};

inline CellButton* asCell(QPushButton* b) { return static_cast<CellButton*>(b); }

}

BoardWidget::BoardWidget(QWidget* parent) : QWidget(parent) {
    grid_ = new QGridLayout(this);
    grid_->setContentsMargins(8, 8, 8, 8);
    grid_->setSpacing(6);
    rebuild();
}

BoardWidget::~BoardWidget() {
    delete[] cells_;
    delete[] weights_;
}

void BoardWidget::setBoardSize(int n) {
    if (n <= 0) return;
    if (n == N_) return;
    N_ = n;
    rebuild();
}

void BoardWidget::resetBoard() {
    for (int i = 0; i < cellsCount_; ++i) {
        if (!cells_[i]) continue;
        cells_[i]->setText("");
        cells_[i]->setEnabled(true);
    }
    updateCellVisuals();
}

void BoardWidget::setCellWeight(int r, int c, int weight) {
    if (r < 0 || c < 0 || r >= N_ || c >= N_) return;
    const int idx = r * N_ + c;
    if (!weights_ || idx < 0 || idx >= cellsCount_) return;
    weights_[idx] = weight;

    if (cells_ && cells_[idx]) {
        asCell(cells_[idx])->setWeight(weight);
        asCell(cells_[idx])->setShowWeight(showWeights_);
    }
}

void BoardWidget::setShowWeights(bool on) {
    showWeights_ = on;
    for (int i = 0; i < cellsCount_; ++i) {
        if (!cells_[i]) continue;
        asCell(cells_[i])->setShowWeight(showWeights_);
    }
}

void BoardWidget::setCellSizePx(int px) {
    cellSizePx_ = px;
    if (cellSizePx_ < 0) cellSizePx_ = 0;
    updateCellVisuals();
}

void BoardWidget::setCellText(int r, int c, const QString& text, bool enabled) {
    if (r < 0 || c < 0 || r >= N_ || c >= N_) return;
    const int idx = r * N_ + c;
    if (!cells_ || idx < 0 || idx >= cellsCount_) return;
    if (!cells_[idx]) return;

    cells_[idx]->setText(text);
    cells_[idx]->setEnabled(enabled);
    updateCellVisuals();
}

void BoardWidget::rebuild() {
    while (QLayoutItem* item = grid_->takeAt(0)) {
        if (QWidget* w = item->widget()) w->deleteLater();
        delete item;
    }

    delete[] cells_;
    delete[] weights_;
    cellsCount_ = N_ * N_;
    cells_ = new QPushButton*[cellsCount_];
    weights_ = new int[cellsCount_];

    for (int i = 0; i < cellsCount_; ++i) {
        cells_[i] = nullptr;
        weights_[i] = 0;
    }

    for (int r = 0; r < N_; ++r) {
        for (int c = 0; c < N_; ++c) {
            CellButton* b = new CellButton(this);
            b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            b->setText("");
            b->setShowWeight(showWeights_);
            b->setWeight(0);

            const int rr = r;
            const int cc = c;
            connect(b, &QPushButton::clicked, this, [this, rr, cc]() {
                emit cellClicked(rr, cc);
            });

            grid_->addWidget(b, r, c);
            cells_[r * N_ + c] = b;
        }
    }

    updateCellVisuals();
}

void BoardWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateCellVisuals();
}

void BoardWidget::updateCellVisuals() {
    if (!cells_ || cellsCount_ == 0) return;

    const int w = width();
    const int h = height();
    int cell = (w < h ? w : h) / (N_ > 0 ? N_ : 1);

    if (cellSizePx_ > 0) cell = std::min(cell, cellSizePx_);

    int px = cell / 2;
    if (px < 12) px = 12;
    if (px > 48) px = 48;

    QFont f;
    f.setBold(true);
    f.setPointSize(px);

    for (int i = 0; i < cellsCount_; ++i) {
        if (!cells_[i]) continue;
        cells_[i]->setFont(f);

        cells_[i]->setMinimumSize(cell, cell);
        cells_[i]->setMaximumSize(cell, cell);
    }
}
