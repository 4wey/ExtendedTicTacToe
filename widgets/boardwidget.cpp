#include "boardwidget.h"

#include <QGridLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QFont>
#include <QSizePolicy>

BoardWidget::BoardWidget(QWidget* parent) : QWidget(parent) {
    grid_ = new QGridLayout(this);
    grid_->setContentsMargins(8, 8, 8, 8);
    grid_->setSpacing(6);
    rebuild();
}

BoardWidget::~BoardWidget() {
    delete[] cells_;
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
    // очистка layout
    while (QLayoutItem* item = grid_->takeAt(0)) {
        if (QWidget* w = item->widget()) w->deleteLater();
        delete item;
    }

    delete[] cells_;
    cellsCount_ = N_ * N_;
    cells_ = new QPushButton*[cellsCount_];
    for (int i = 0; i < cellsCount_; ++i) cells_[i] = nullptr;

    for (int r = 0; r < N_; ++r) {
        for (int c = 0; c < N_; ++c) {
            QPushButton* b = new QPushButton(this);
            b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            b->setText("");

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
    const int cell = (w < h ? w : h) / (N_ > 0 ? N_ : 1);
    int px = cell / 2;
    if (px < 12) px = 12;
    if (px > 48) px = 48;

    QFont f;
    f.setBold(true);
    f.setPointSize(px);

    for (int i = 0; i < cellsCount_; ++i) {
        if (!cells_[i]) continue;
        cells_[i]->setFont(f);
    }
}
