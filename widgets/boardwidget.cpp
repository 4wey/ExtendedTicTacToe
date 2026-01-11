#include "widgets/boardwidget.h"

#include <QGridLayout>
#include <QPushButton>
#include <QPainter>
#include <QStyleOptionButton>

// Кнопка клетки с отрисовкой веса поверх
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

        if (!showWeight_) return;
        // 0 можно не рисовать (или рисовать — по желанию)
        // если хочешь рисовать и нули — убери условие
        if (weight_ == 0) return;

        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);

        QFont f = font();
        // маленький, но читаемый
        int pt = qMax(8, height() / 6);
        f.setPointSize(pt);
        p.setFont(f);

        // лёгкая “подпись” в углу
        p.setPen(QColor(120, 120, 120));

        const QRect r = rect().adjusted(0, 0, -6, -4);
        p.drawText(r, Qt::AlignRight | Qt::AlignBottom, QString::number(weight_));
    }

private:
    int weight_ = 0;
    bool showWeight_ = false;
};

BoardWidget::BoardWidget(QWidget* parent) : QWidget(parent) {
    grid_ = new QGridLayout(this);
    grid_->setContentsMargins(0, 0, 0, 0);
    grid_->setSpacing(4);

    setBoardSize(3);
}

void BoardWidget::setIcons(const QIcon& xIcon, const QIcon& oIcon) {
    xIcon_ = xIcon;
    oIcon_ = oIcon;

    // обновим иконки на поле
    for (int r = 0; r < N_; ++r) {
        for (int c = 0; c < N_; ++c) {
            setCellOwner(r, c, owners_[idx(r, c)], buttons_[idx(r, c)]->isEnabled());
        }
    }
}

void BoardWidget::setCellSizePx(int px) {
    cellSizePx_ = qMax(24, px);

    // применить на все кнопки
    const QSize iconSz(qMax(16, cellSizePx_ - 22), qMax(16, cellSizePx_ - 22));
    for (int i = 0; i < buttons_.size(); ++i) {
        buttons_[i]->setFixedSize(cellSizePx_, cellSizePx_);
        buttons_[i]->setIconSize(iconSz);
    }

    updateGeometry();
}

void BoardWidget::setBoardSize(int n) {
    N_ = qMax(1, n);
    owners_.fill(0, N_ * N_);
    weights_.fill(0, N_ * N_);
    rebuild();
}

void BoardWidget::rebuild() {
    // очистка layout
    while (QLayoutItem* item = grid_->takeAt(0)) {
        if (QWidget* w = item->widget()) w->deleteLater();
        delete item;
    }

    buttons_.clear();
    buttons_.resize(N_ * N_);

    const QSize iconSz(qMax(16, cellSizePx_ - 22), qMax(16, cellSizePx_ - 22));

    for (int r = 0; r < N_; ++r) {
        for (int c = 0; c < N_; ++c) {
            auto* b = new CellButton(this);
            b->setFixedSize(cellSizePx_, cellSizePx_);
            b->setIconSize(iconSz);
            b->setFocusPolicy(Qt::NoFocus);

            const int k = idx(r, c);
            buttons_[k] = b;

            // веса + режим отображения
            static_cast<CellButton*>(b)->setWeight(weights_[k]);
            static_cast<CellButton*>(b)->setShowWeight(showWeights_);

            connect(b, &QPushButton::clicked, this, [this, r, c]() {
                emit cellClicked(r, c);
            });

            grid_->addWidget(b, r, c);
        }
    }

    // восстановим владельцев (иконки)
    for (int r = 0; r < N_; ++r) {
        for (int c = 0; c < N_; ++c) {
            setCellOwner(r, c, owners_[idx(r, c)], true);
        }
    }
}

void BoardWidget::setCellOwner(int r, int c, int owner, bool enabled) {
    if (!inBounds(r, c)) return;
    const int k = idx(r, c);

    owners_[k] = owner;

    auto* b = buttons_[k];
    b->setEnabled(enabled);

    if (owner == 1) {
        b->setIcon(xIcon_);
    } else if (owner == -1) {
        b->setIcon(oIcon_);
    } else {
        b->setIcon(QIcon());
    }
}

void BoardWidget::setCellWeight(int r, int c, int weight) {
    if (!inBounds(r, c)) return;
    const int k = idx(r, c);
    weights_[k] = weight;

    if (auto* cb = dynamic_cast<CellButton*>(buttons_[k])) {
        cb->setWeight(weight);
    }
}

void BoardWidget::setWeights(const QVector<int>& weights) {
    if (weights.size() != N_ * N_) return;
    weights_ = weights;

    for (int i = 0; i < buttons_.size(); ++i) {
        if (auto* cb = dynamic_cast<CellButton*>(buttons_[i])) {
            cb->setWeight(weights_[i]);
        }
    }
}

void BoardWidget::setShowWeights(bool on) {
    showWeights_ = on;

    for (int i = 0; i < buttons_.size(); ++i) {
        if (auto* cb = dynamic_cast<CellButton*>(buttons_[i])) {
            cb->setShowWeight(on);
        }
    }
}
