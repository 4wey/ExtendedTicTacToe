#pragma once

#include <QDialog>

class QLabel;

class StatsDialog : public QDialog {
    Q_OBJECT
public:
    explicit StatsDialog(QWidget* parent = nullptr);

    void setStats(int games, int xWins, int oWins, int draws, bool ru);

private:
    QLabel* label_ = nullptr;
};
