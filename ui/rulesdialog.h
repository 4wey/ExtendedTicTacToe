#pragma once

#include <QDialog>
#include "game/game_types.h"
#include "game/game_engine.h"

class QTextEdit;

class RulesDialog : public QDialog {
    Q_OBJECT
public:
    explicit RulesDialog(QWidget* parent = nullptr);

    void setRules(GameMode mode, FillMode fill, GameConfig cfg, bool ru);

private:
    QTextEdit* text_ = nullptr;
};
