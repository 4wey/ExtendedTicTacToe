#include "statsdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

StatsDialog::StatsDialog(QWidget* parent) : QDialog(parent) {
    setModal(true);
    setMinimumWidth(320);

    auto* layout = new QVBoxLayout(this);
    label_ = new QLabel(this);
    label_->setWordWrap(true);

    auto* btnClose = new QPushButton("OK", this);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);

    layout->addWidget(label_);
    layout->addWidget(btnClose);
}

void StatsDialog::setStats(int games, int xWins, int oWins, int draws, bool ru) {
    setWindowTitle(ru ? "Статистика" : "Statistics");

    QString text;
    if (ru) {
        text = QString("Сыграно игр: %1\nПобед X: %2\nПобед O: %3\nНичьих: %4")
               .arg(games).arg(xWins).arg(oWins).arg(draws);
    } else {
        text = QString("Games played: %1\nX wins: %2\nO wins: %3\nDraws: %4")
               .arg(games).arg(xWins).arg(oWins).arg(draws);
    }
    label_->setText(text);
}
