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
        return;
    }

    if (mode == GameMode::Ultimate) {
        text_->setText(
            "Ultimate TicTacToe:\n"
            "1) Поле 9×9 разделено на 9 малых полей 3×3.\n"
            "2) Первый ход можно сделать в любую клетку.\n"
            "3) Следующий игрок обязан ходить в то малое поле, которое соответствует позиции клетки предыдущего хода внутри малого поля.\n"
            "   Например, если сходили в левый верхний угол малого поля, соперник должен ходить в левое верхнее малое поле.\n"
            "4) Если требуемое малое поле уже выиграно или полностью заполнено, можно ходить в любое доступное малое поле.\n"
            "5) Малое поле выигрывается как обычные крестики-нолики 3×3.\n"
            "6) Побеждает игрок, который выиграл три малых поля подряд по горизонтали, вертикали или диагонали на большом поле.\n"
            "7) Если ходов больше нет и победителя нет, фиксируется ничья."
        );
        return;
    }

    text_->setText("Для этого режима отдельные правила не предусмотрены.");
    return;
}

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
    return;
}

if (mode == GameMode::Ultimate) {
    text_->setText(
        "Ultimate TicTacToe:\n"
        "1) The 9×9 board is split into 9 local 3×3 boards.\n"
        "2) The first move can be made in any cell.\n"
        "3) The next player must play in the local board that matches the position of the previous move inside its local board.\n"
        "4) If that local board is already won or full, you may play in any available local board.\n"
        "5) A local board is won like a normal 3×3 tic-tac-toe.\n"
        "6) You win by winning three local boards in a row on the big 3×3 board (row/column/diagonal).\n"
        "7) If no moves remain and there is no winner, the game is a draw."
    );
    return;
}

text_->setText("No separate rules for this mode.");


}