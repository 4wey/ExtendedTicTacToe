#include "rulesdialog.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>

RulesDialog::RulesDialog(QWidget* parent) : QDialog(parent) {
    setModal(true);
    setMinimumSize(520, 420);

    auto* layout = new QVBoxLayout(this);

    text_ = new QTextEdit(this);
    text_->setReadOnly(true);

    auto* btnClose = new QPushButton("OK", this);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);

    layout->addWidget(text_, 1);
    layout->addWidget(btnClose, 0);
}

void RulesDialog::setRules(GameMode mode, FillMode fill, GameConfig cfg, bool ru) {
    setWindowTitle(ru ? "Правила" : "Rules");

    if (mode == GameMode::Classic3x3) {
        text_->setText(ru
            ? "Классика 3×3: игроки ставят X и O по очереди. Побеждает тот, кто первым соберёт линию из 3."
            : "Classic 3×3: players place X and O. First to get 3 in a row wins.");
        return;
    }

    QString fillTxtRu, fillTxtEn;
    switch (fill) {
        case FillMode::Free: fillTxtRu = "Свободно"; fillTxtEn = "Free"; break;
        case FillMode::TopDownRows: fillTxtRu = "Сверху-вниз (активная строка)"; fillTxtEn = "Top-down (active row)"; break;
        case FillMode::LeftRightCols: fillTxtRu = "Слева-направо (активный столбец)"; fillTxtEn = "Left-right (active col)"; break;
        case FillMode::RandomRow: fillTxtRu = "Случайная строка"; fillTxtEn = "Random row"; break;
        case FillMode::RandomCol: fillTxtRu = "Случайный столбец"; fillTxtEn = "Random col"; break;
        case FillMode::RandomRowOrCol: fillTxtRu = "Случайная строка ИЛИ столбец"; fillTxtEn = "Random row OR col"; break;
    }

    if (ru) {
        text_->setText(
            "Score mode — режим «на очки», а не «до победы».\n\n"
            + QString("Поле: %1×%1\n").arg(cfg.boardSize)
            + QString("Длина линии: %1\n").arg(cfg.winLen)
            + QString("Лимит ходов: %1\n").arg(cfg.maxMoves)
            + QString("Ограничение заполнения: %1\n\n").arg(fillTxtRu)
            +
            "Как считаются очки:\n"
            "1) Каждая клетка имеет целочисленный вес.\n"
            "2) Линия длины L (например 5 подряд) даёт очки = сумме весов клеток линии.\n"
            "3) Есть стоимость ходов (I.4.3):\n"
            "   - X: 2,4,6,8,...\n"
            "   - O: 3,5,7,9,...\n"
            "4) Итог = (очки за линии) − (сумма стоимостей ходов).\n"
            "5) После лимита ходов сравниваем итог X и O — у кого больше, тот победил.\n"
        );
    } else {
        text_->setText(
            "Score mode is points-based, not instant win.\n\n"
            + QString("Board: %1×%1\n").arg(cfg.boardSize)
            + QString("Line length: %1\n").arg(cfg.winLen)
            + QString("Move limit: %1\n").arg(cfg.maxMoves)
            + QString("Fill restriction: %1\n\n").arg(fillTxtEn)
            +
            "Scoring:\n"
            "1) Each cell has an integer weight.\n"
            "2) A line of length L grants points = sum of the weights of its cells.\n"
            "3) Move cost (I.4.3):\n"
            "   - X: 2,4,6,8,...\n"
            "   - O: 3,5,7,9,...\n"
            "4) Total = (line points) − (sum of move costs).\n"
            "5) After move limit, compare totals — higher total wins.\n"
        );
    }
}
