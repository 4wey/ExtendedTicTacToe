#include "rulesdialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

RulesDialog::RulesDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Rules");
    setMinimumSize(520, 360);

    auto* l = new QVBoxLayout(this);

    text_ = new QLabel(this);
    text_->setWordWrap(true);

    auto* btn = new QPushButton("OK", this);
    connect(btn, &QPushButton::clicked, this, &QDialog::accept);

    l->addWidget(text_, 1);
    l->addWidget(btn, 0);
}

void RulesDialog::setMode(GameMode mode, bool russian) {
    if (russian) {
        setWindowTitle("Правила");
        if (mode == GameMode::Score10x10) {
            text_->setText(
                "Score mode:\n"
                "1) Игра идёт на поле 10×10, длина линии — 4.\n"
                "2) У каждой клетки есть вес (число). Стоимость линии — сумма весов клеток линии.\n"
                "3) Стоимость фигур: X — только чётные (2,4,6...), O — только нечётные (3,5,7...).\n"
                "4) Итог: total = lineScore - spent.\n"
                "5) В режиме заполнения может быть активная строка/столбец: ходить можно только туда."
            );
        } else {
            text_->setText("Для этого режима отдельные правила не предусмотрены.");
        }
    } else {
        setWindowTitle("Rules");
        if (mode == GameMode::Score10x10) {
            text_->setText(
                "Score mode:\n"
                "1) Board 10×10, line length = 4.\n"
                "2) Each cell has an integer weight. Line value = sum of weights in that line.\n"
                "3) Piece costs: X uses even costs (2,4,6...), O uses odd costs (3,5,7...).\n"
                "4) Total: total = lineScore - spent.\n"
                "5) Filling modes can restrict moves to an active row/column."
            );
        } else {
            text_->setText("No separate rules for this mode.");
        }
    }
}
