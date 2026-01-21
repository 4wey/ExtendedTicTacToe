#pragma once

#include <QDialog>
#include "game/game_types.h"

class QLabel;

class RulesDialog : public QDialog {
    Q_OBJECT
public:
    explicit RulesDialog(QWidget* parent = nullptr);

    void setMode(GameMode mode, bool russian);

private:
    QLabel* text_ = nullptr;
};
